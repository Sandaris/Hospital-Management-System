#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>

///////////////////////////////////ULTILITIES///////////////////////////////////

/*This function open the file for you, check the existance of file in the data folder, if file doesnt exist, raise a wrong filename error and prevent creating a new file, lastly return a file pointer of the file you opened.
* 
*Parameter:
* - filename: The name of the file you want to open. (without .txt)
* - mode : file mode you want to open the file with.
*
* Returns: A pointer to the file.
*/
FILE* filecheck(const char* filename, const char* mode)
{
    //get Folder Path
    char folderPath[1024];
    if(getcwd(folderPath, sizeof(folderPath)) == NULL)
    {
        perror("getcwd() error");
        return NULL;
    }

    //strcat the assignment folder path with data folder
    char dataPath[1024];
    sprintf(dataPath, "%s\\data", folderPath);
    
    //strcat the data folder path with filename
    char filePath[1024];
    sprintf(filePath, "%s\\%s.txt", dataPath, filename);

    //check the existance of file in the data folder, if file doesnt exist, raise a wrong filename error and prevent creating a new file
    if(access(filePath, F_OK) == -1)
    {
        fprintf(stderr, "Error: Check Your File Name!!\nFilename %s in folder: %s does not exist!\n", filename, dataPath);
        return NULL;
    }
    
    //Open the File
    FILE* filePointer = fopen(filePath, mode);

    //Open file error check
    if(filePointer == NULL)
    {
        printf("Error opening file %s!\n", filename);
        return NULL;
    }
    return filePointer;
}

/*A 2D data container to store entire data of the file.

you use this 2Dcontainer to store data from **getData() most of the time.

Implementaton:

*- call  struct dataContainer2D <container_name> = getData( file_name );
    -- container_name: is the name of the struct
    -- file_name: is the name of the file you want to read without .txt
    
To use the element in the struct:
    * - container_name.field_name : return you the field_names in the file 
    * - container_name.data : a 2D array containing each line from the file
    * - container_name.y : number of rows / horizontal lines count of the file 
    * - container_name .x : number of columns / vertical lines count of the file

*/
struct dataContainer2D 
{
    int error; // 1 - error | 0 - fine
    char** fields; // an array containing the fields
    char*** data; // a 2D array containing each line of data
    int y; //y - number of rows / lines / arrays in the 2D array
    int x; //x - number of columns / elements in each array
};

/*A 1D data container to store a single line of data from  the file.

you use this 1Dcontainer to store data( single line ) from : queryField(), queryKey(), etc. 

Implementaton:
*- call  struct dataContainer1D <container_name> = queryField( file_name, field_name );
    -- container_name: is the name of the struct
    -- file_name: is the name of the file you want to read without .txt
    -- field_name: is the name of the field you want to read from the file
    
To use the element in the struct:
    * - container_name.field_name : return you the field_names in the file 
    * - container_name.data : a 1D contain specific line from the file according to the field_name you put
    * - container_name . x : number of columns / vertical lines count of the file / number of elements in the line

*/
struct dataContainer1D 
{
    int error; // 1 - error | 0 - fine
    char** fields; // an array containing the fields
    char** data; // an array containing one line or one column of data
    int x; // x - number of columns / elements in the array
};

void clearTerminal() 
{
    printf("\e[1;1H\e[2J");
}

char* getString(char* prompt) 
{
    int bufferLength = 256;
    char buffer[bufferLength];

    printf(prompt);
    fgets(buffer, bufferLength, stdin);

    // Remove New Line
    buffer[strcspn(buffer, "\n")] = 0;

    return strdup(buffer);
}

int getInt(char* prompt) 
{
    int bufferLength = 256;
    char buffer[bufferLength];

    printf(prompt);
    fgets(buffer, bufferLength, stdin);

    buffer[strcspn(buffer, "\n")] = 0;

    // Decides if string is a Number
    int isDigit = 1;
    for (int i = 0; buffer[i] && isDigit; i++) {
        isDigit = isdigit(buffer[i]);
    }

    if (isDigit) {
        return atoi(buffer);
    }

    return -1;
}


float getFloat(char* prompt) 
{
    int bufferLength = 256;
    char buffer[bufferLength];

    printf(prompt);
    fgets(buffer, bufferLength, stdin);

    buffer[strcspn(buffer, "\n")] = 0;

    // Decides if string is a Number
    int isDigit = 1;
    int numNotDigit = 1;
    for (int i = 0; buffer[i] && isDigit; i++) {
        if (buffer[i] == '.' && numNotDigit) {
            numNotDigit = 0;
            continue;
        }
        isDigit = isdigit(buffer[i]);
    }

    if (isDigit) {
        return atof(buffer);
    }

    return -1.0;
}

/* This function frees the memory allocated for the dataContainer2D struct.
 * It iterates over each line of data, frees the memory allocated for each element in that line,
 * and then frees the memory allocated for the line itself. Finally, it frees the memory
 * allocated for the fields and the data array.
 */
void freeMalloc2D(struct dataContainer2D pointer) 
{
    for (int i = 0; i < pointer.y; i++) {
        free(pointer.data[i]);
    }
    free(pointer.data);
    free(pointer.fields);
}

// Frees the memory allocated for the dataContainer1D struct
void freeMalloc1D(struct dataContainer1D pointer) 
{
    free(pointer.data);
    free(pointer.fields);
}

struct dataContainer2D concatDataContainer(struct dataContainer2D data1, struct dataContainer2D data2, char* keyField1, char* keyField2) {
    struct dataContainer2D returnedValue;
    returnedValue.error = 0;
    
    int wantedFieldIndex1 = -1;
    int wantedFieldIndex2 = -1;

    for (int i=0; i<data1.x; i++) {
        if (!strncmp(data1.fields[i], keyField1, 256)) {
            wantedFieldIndex1 = i;
            break;
        }
    }

    if (wantedFieldIndex1 == -1) {
        returnedValue.error = 1;
        return returnedValue;
    }

    for (int i=0; i<data2.x; i++) {
        if (!strncmp(data2.fields[i], keyField2, 256)) {
            wantedFieldIndex2 = i;
            break;
        }
    }

    if (wantedFieldIndex2 == -1) {
        returnedValue.error = 1;
        return returnedValue;
    }

    returnedValue.y = data1.y;
    returnedValue.x = data1.x + data2.x -1;

    returnedValue.fields = malloc(returnedValue.x * sizeof(char*));

    for (int i=0; i<data1.x; i++) {
        returnedValue.fields[i] = strdup(data1.fields[i]);
    }

    int count = 0;
    for (int k=0; k<data2.x; k++) {
                    
        if (k == wantedFieldIndex2) {
            continue;
        }
        
        returnedValue.fields[ count + data1.x ] = strdup(data2.fields[k]);
        count++;
    }

    returnedValue.data = malloc(returnedValue.y * sizeof(char**));

    for (int i=0; i<data1.y; i++) {

        returnedValue.data[i] = malloc(returnedValue.x * sizeof(char*));

        for (int k=0; k<data1.x; k++) {
            returnedValue.data[i][k] = strdup(data1.data[i][k]);
        }

        for (int k=data1.x; k<returnedValue.x; k++) {
            returnedValue.data[i][k] = "-";
        }

        for (int j=0; j<data2.y; j++) {
            if (!strncmp(data1.data[i][wantedFieldIndex1], data2.data[j][wantedFieldIndex2], 256)) {
                int count = 0;
                for (int k=0; k<data2.x; k++) {
                    
                    if (k == wantedFieldIndex2) {
                        continue;
                    }

                    returnedValue.data[i][ count + data1.x ] = strdup(data2.data[j][k]);
                    count++;
                }
                break;
            }

        }
    }

    return returnedValue;
}

struct dataContainer2D filterDataContainer(struct dataContainer2D data, char* field, char* key) 
{
    struct dataContainer2D returnedValue;
    returnedValue.error = 0;
    returnedValue.x = data.x;

    int wantedFieldIndex = -1;

    for (int i=0; i<data.x; i++) {
        if (!strncmp(data.fields[i], field, 256)) {
            wantedFieldIndex = i;
            break;
        }
    }

    if (wantedFieldIndex == -1) {
        returnedValue.error = 1;
        return returnedValue;
    }

    int count = 0;
    char** pointerBuffer[data.y];

    for (int i=0; i<data.y; i++) {
        if (!strncmp(data.data[i][wantedFieldIndex], key, 256)) {
            pointerBuffer[count++] = data.data[i];
        }
    }

    if (count == 0) {
        returnedValue.error = 1;
        return returnedValue;
    }

    returnedValue.fields = malloc (data.x * sizeof(char*));
    
    for (int i=0; i<data.x; i++) {
        returnedValue.fields[i] = strdup(data.fields[i]);
    }

    char*** table = malloc (count * sizeof(char**));

    for (int i=0; i<count; i++) {
        table[i] = malloc(data.x * sizeof(char*));

        for (int j=0; j<data.x; j++) {
            table[i][j] = strdup(pointerBuffer[i][j]);
        }
    }

    returnedValue.data = table;
    returnedValue.y = count;

    return returnedValue;
}

struct dataContainer1D getFieldValues(struct dataContainer2D data, char* field) {
    struct dataContainer1D returnedValue;
    returnedValue.error = 0;

    if (data.error) {
        returnedValue.error = 1;
        return returnedValue;
    }

    int fieldColumn = -1;

    for (int i=0; i<data.x; i++) 
    {
        if (!strncmp(data.fields[i], field, 255))
        {
            fieldColumn = i;
            break;
        }
    }

    // Fail to find
    if (fieldColumn == -1) 
    {
        returnedValue.error = 1;
        return returnedValue;
    }

    // Get all Data In the Specified Field Column
    char** fieldData = malloc (data.y * sizeof(char*));

    for (int i=0; i<data.y; i++) 
    {
        fieldData[i] = strdup(data.data[i][fieldColumn]);
    }

    returnedValue.data = fieldData;
    returnedValue.x = data.y;

    char** fieldName = malloc (1 * sizeof(char*));

    fieldName[0] = field;

    returnedValue.fields = fieldName;

    return returnedValue;
}

struct dataContainer2D shortenDataContainer(struct dataContainer2D data, char* wantedFields[], int numFields) {
    struct dataContainer2D returnedValue;
    returnedValue.error = 0;
    returnedValue.x = numFields;
    returnedValue.y = data.y;

    int wantedFieldIndexes[numFields];

    for (int i=0; i < numFields; i++) {
        int valid = 0;
        for (int j=0; j<data.x; j++) {
            if (!strncmp(data.fields[j], wantedFields[i], 256)) {
                wantedFieldIndexes[i] = j;
                valid = 1;
        }
    }

        if (!valid) {
        returnedValue.error = 1;
        return returnedValue;
    }
    }

    returnedValue.fields = malloc (returnedValue.x * sizeof(char*));

    for (int i=0; i<numFields; i++) {
        returnedValue.fields[i] = strdup(wantedFields[i]);
    }
    

    char*** table = malloc (returnedValue.y * sizeof(char**));

    for (int i=0; i < returnedValue.y; i++) {
        
        table[i] = malloc (returnedValue.x * sizeof(char*));

        for (int j=0; j < returnedValue.x; j++) {
            table[i][j] = strdup(data.data[i][wantedFieldIndexes[j]]); 
        }
    }

    returnedValue.data = table;

    return returnedValue;

}
///////////////////////////////////DISPLAY FUNCTION//////////////////////////////////

/*Display a system message on the terminal and wait for a specified duration before clearing the terminal.*/
void displaySystemMessage(char* message, int waitTime) {
    clearTerminal();
    printf(message);
    sleep(waitTime);
    clearTerminal();
}

/*Print Column Vertically in a table

char* options[] = {display_CaseName, display_DiagnosticComments}

displayUnorderedOptions(CaseHeader, options, 2);*/
void displayUnorderedOptions(char* header, char* options[], int noOptions) 
{
    // get max sizeof option string
    int maxLength = strlen(header);

    for (int i=0; i<noOptions; i++) {
        // get length
        int currentLength = strlen(options[i]);
    
        if (currentLength > maxLength) {
            maxLength = currentLength;
        }
    }

    const int horizontalpadding = 15;
    int borderLength = maxLength + 2 * horizontalpadding;

    // Top  Horizontal Line
    for (int i=0; i<borderLength; i++) {
        printf("-");
    }
    printf("\n");

    // Header
    const int verticalPadding = 1;

    int headerPadding = floor((borderLength - strlen(header) - 2)/2);
    
    const int borderCount = 2;
    const char character = '|';
    const int rightPadding = borderLength - borderCount - strlen(header) - headerPadding;

    printf("%c", character);

    for (int i=0; i<headerPadding; i++) {
        printf(" ");
    }

    printf(header);

    for (int i=0; i<rightPadding; i++) {
        printf(" ");
    }

    printf("%c", character);

    printf("\n");

    // Middle Horizontal Line
    for (int i=0; i<borderLength; i++) {
        printf("-");
    }
    printf("\n");

    const int leftPadding = 5;

    // Options
    for (int i=0; i<noOptions; i++) {
        const char* text = options[i];
        const int rightPadding = borderLength - borderCount - strlen(text) - leftPadding;

        printf("%c", character);

        for (int i=0; i<leftPadding; i++) {
            printf(" ");
        }

        printf(text);

        for (int i=0; i<rightPadding; i++) {
            printf(" ");
        }

        printf("%c", character);

        printf("\n");
    }

    // Bottom Horizontal Line
    for (int i=0; i<borderLength; i++) {
        printf("-");
    }
    printf("\n");
}

/*Print Column Vertically with index 
1.
2.
3.

*/
void displayOptions(char* header, char* options[], int noOptions) 
{
    // get max sizeof option string
    int maxLength = strlen(header);
    char* modifiedOptions[noOptions];

    for (int i=0; i<noOptions; i++) {
        // format the strings
        char buffer[255];

        sprintf(buffer, "%d. %s", i+1, options[i]);

        modifiedOptions[i] = strdup(buffer);

        // get length
        int currentLength = strlen(modifiedOptions[i]);
    
        if (currentLength > maxLength) {
            maxLength = currentLength;
        }
    }

    const int horizontalpadding = 15;
    int borderLength = maxLength + 2 * horizontalpadding;

    // Top  Horizontal Line
    for (int i=0; i<borderLength; i++) {
        printf("-");
    }
    printf("\n");

    // Header
    const int verticalPadding = 1;

    int headerPadding = floor((borderLength - strlen(header) - 2)/2);
    
    const int borderCount = 2;
    const char character = '|';
    const int rightPadding = borderLength - borderCount - strlen(header) - headerPadding;

    printf("%c", character);

    for (int i=0; i<headerPadding; i++) {
        printf(" ");
    }

    printf(header);

    for (int i=0; i<rightPadding; i++) {
        printf(" ");
    }

    printf("%c", character);

    printf("\n");

    // Middle Horizontal Line
    for (int i=0; i<borderLength; i++) {
        printf("-");
    }
    printf("\n");

    const int leftPadding = 5;

    // Options
    for (int i=0; i<noOptions; i++) {
        const char* text = modifiedOptions[i];
        const int rightPadding = borderLength - borderCount - strlen(text) - leftPadding;

        printf("%c", character);

        for (int i=0; i<leftPadding; i++) {
            printf(" ");
        }

        printf(text);

        for (int i=0; i<rightPadding; i++) {
            printf(" ");
        }

        printf("%c", character);

        printf("\n");
    }

    // Bottom Horizontal Line
    for (int i=0; i<borderLength; i++) {
        printf("-");
    }
    printf("\n");
}

/*char* options[] = {"ar", "a", "b", "C"};

    displayMenu("GOD", options, 4);*/
int displayMenu(char* header, char* options[], int noOptions) {
    displayOptions(header, options, noOptions);

    // Get Input
    int bufferLength = 256;
    char input[bufferLength];
    printf("Enter your Input: ");

    fgets(input, bufferLength, stdin);   
    input[strcspn(input, "\n")] = 0;

    // Decides if string is a Number
    int isDigit = 1;
    for (int i = 0; input[i] && isDigit; i++) {
        isDigit = isdigit(input[i]);
    }

    // Checks f input is a Number and is Valid
    int intInput = atoi(input);
    if (isDigit && 0 < intInput && intInput <= noOptions) {
        return intInput;
    }

    // Lowercase the input string
    for (int j=0; input[j]; j++) {
        input[j] = tolower(input[j]);
    }


    // Repeat Menu until Valid Input
    for (int i=0; i<noOptions; i++) {
        char* option = strdup(options[i]);

        // Lowercase the option string
        for (int j=0; option[j]; j++) {
            option[j] = tolower(option[j]);
        }

        // Compare the strings
        if (!strncmp(input, option, bufferLength)) {
            return i+1;
        }
    }

    // Repeat Menu until Valid Input
    displaySystemMessage("INSERT THE CORRECT INPUT...\n\nWaiting For 3 Seconds.", 3);
    return displayMenu(header, options, noOptions);
}

/* This function display your data in a table form.

struct dataContainer2D d_appointments = queryFieldStrict("Appointments", "StaffUserID",doctor_username);
        
    displayTabulatedData(d_appointments);
    freeMalloc2D(d_appointments);*/
 void displayTabulatedData(struct dataContainer2D data) {
    const int minPadding = 5;
    int numRow = data.y + 1;
    char** displayedStrings = malloc (numRow * sizeof(char*));

    // Find Max Column Lengths
    int columnLengths[data.x];

    for (int i=0; i<data.x; i++) {
        int columnLength = strlen(data.fields[i]);

        for (int j=0; j<data.y; j++) {
            int currentLength = strlen(data.data[j][i]);

            if (currentLength > columnLength) {
                columnLength = currentLength;
            }
        }

        columnLengths[i] = columnLength;
    }

    int bufferLength = 500;
    char stringBuffer[bufferLength];

    // Clear String;
    stringBuffer[0] = '\0';

    // Format the Field Strings
    for (int i=0; i<data.x; i++) {
        int totalPadding = columnLengths[i] - strlen(data.fields[i]) + minPadding;

        int leftPadding = floor(totalPadding/2);
        
        for (int n=0; n<leftPadding; n++) {
            strncat(stringBuffer, " ", 2);
        } 

        strncat(stringBuffer, data.fields[i], strlen(data.fields[i]));

        int rightPadding = totalPadding - leftPadding;

        for (int n=0; n<rightPadding; n++) {
            strncat(stringBuffer, " ", 2);
        } 

        strncat(stringBuffer, "|", 2);
    }

    displayedStrings[0] = strdup(stringBuffer);

    // Format the Strings

    for (int i=0; i<data.y; i++) {
        stringBuffer[0] = '\0';

        // Create String
        for (int j=0; j<data.x; j++) {
            int totalPadding = columnLengths[j] - strlen(data.data[i][j]) + minPadding;

            // Left Padding
            int leftPadding = floor(totalPadding/2);
            
            for (int n=0; n<leftPadding; n++) {
                strncat(stringBuffer, " ", 2);
            } 

            strncat(stringBuffer, data.data[i][j], strlen(data.data[i][j]));

            // Right Padding
            int rightPadding = totalPadding - leftPadding;

            for (int n=0; n<rightPadding; n++) {
                strncat(stringBuffer, " ", 2);
            } 

            strncat(stringBuffer, "|", 2);
        }

        // Add String to Array
        displayedStrings[i+1] = strdup(stringBuffer);
    }

    int tableLength = strlen(displayedStrings[0]) + 1;

    for (int i=0; i<tableLength; i++) {
        printf("-");
    }

    for (int i=0; i<numRow; i++) {
        printf("\n|%s\n", displayedStrings[i]);
        
        for (int i=0; i<tableLength; i++) {
            printf("-");
        }
    }

    printf("\n");

    free(displayedStrings);
}

///////////////////////////////////*DATA READ FUNCTIONS*////////////////////////////////////////////////

// returns a struct that holds the field and data of the corresponding file
struct dataContainer2D getData(const char* filename) 
{
    int bufferLength = 255;
    char buffer[bufferLength]; 

    const char* separator = ";";

    struct dataContainer2D container;
    container.error = 0;

    FILE* filePointer = filecheck(filename, "r");

    if (filePointer == NULL) {
        container.error = 1;
        return container;
    }

    const int fieldColumn = 1;

    // Get Dimensions of the Array
    int fileLength = -1; // y
    int columnLength = -1; // x

    // get File Length - Y
    while(fgets(buffer, bufferLength, filePointer)) 
    {
        fileLength++;
    }

    rewind(filePointer);

    // get Column Length - X
    fgets(buffer, bufferLength, filePointer);

    // Tokenize the line based on the separators
    char* token = strtok(buffer, separator);

    // Keep counting the number of separators until there are no more
    while (token != NULL) 
    {
        // Increment the column counter
        columnLength++;

        // Get the next token
        token = strtok(NULL, separator);
    }

    rewind(filePointer);

    // Create Array of Pointers
    char*** data = malloc (fileLength * sizeof(char**)); 

    // Place Pointers of Arrays into the Array to Make a 2D array
    for (int i=0; i < fileLength; i++) 
    {
        data[i] = malloc (columnLength * sizeof(char*));
    }

    // get Fields
    char** fields = malloc (columnLength * sizeof(char*));

    fgets(buffer, bufferLength, filePointer);
    buffer[strcspn(buffer, "\n")] = 0;
    token = strtok(buffer, separator);
    int j=0;

    while (token != NULL) 
    {
        fields[j++] = strdup(token);
        token = strtok(NULL, separator);
    }

    container.fields = fields;

    // Get Data
    int i=0;

    while(fgets(buffer, bufferLength, filePointer)) 
    { // Reads line by line until end of file
        buffer[strcspn(buffer, "\n")] = 0;
        char* token = strtok(buffer, separator);
        int j=0;

        while (token != NULL) 
        {
            data[i][j++] = strdup(token);
            token = strtok(NULL, separator);
        }

        i++;
    }
    fclose(filePointer);

    container.data = data;
    container.x = columnLength;
    container.y = fileLength;

    return container;
}

/*____________________________3 Functions that you can call___________________________________________________________________________________*/

/*Horizontal search using unique_key, return a line of values with that unique_key.
    * - example: 
        -- unique_key = POTATO1
        returns in .data: 
                    * - POTATO1;password123;John Smith;Doctor;
 
To call this function:
    struct dataContainer1D <container_name> = queryKey(file_name, unique_key );
    -- container_name: is the name of the struct
    -- file_name: name of the file that you want to read
    -- unique_Key: the unique key for the line you want to search for


 */
struct dataContainer1D queryKey(const char* filename, char* key) 
{
    struct dataContainer2D data = getData(filename);

    struct dataContainer1D returnedValue;
    returnedValue.error = 0;

    if (data.error) {
        freeMalloc2D(data);
        returnedValue.error = 1;
        return returnedValue;
    }


    // Get all Data In the Specified Field Column
    char** IDs = malloc (data.y * sizeof(char*));

    const int fieldColumn = 0;

    for (int i=0; i<data.y; i++) 
    {
        IDs[i] = strdup(data.data[i][fieldColumn]);
    }

    char** relayDataArray = malloc (data.x * sizeof(char*));
    char** relayFieldArray = malloc (data.x * sizeof(char*));


    for (int i=0; i<data.y; i++) 
    {
        if (!strncmp(IDs[i], key, 255)) 
        {
            // Copy Data to Relay Array
            for (int j=0; j<data.x; j++) {
                relayDataArray[j] = strdup(data.data[i][j]);
                relayFieldArray[j] = strdup(data.fields[j]);
            }

            returnedValue.data = relayDataArray;
            returnedValue.fields = relayFieldArray;
            returnedValue.x = data.x;

            freeMalloc2D(data);

            return returnedValue;
        }
        
    }

    free(IDs);
    freeMalloc2D(data);
    returnedValue.error = 1;
    return returnedValue;
}

/*Vertical search using field_name, return a line of values with that field_name
    * - example: struct dataContainer1D UserPW = queryField("users", "UserPW");
        -- file : users.txt
        -- field_name = UserPW
        -- returns : all password values

To call this function:A
    struct dataContainer1D <container_name> = queryField(file_name, field );
    -- container_name: is the name of the struct
    -- file_name: name of the file that you want to read
    -- field: the field you want to use for the search
*/
struct dataContainer1D queryField(const char* filename, char* field) 
{
    struct dataContainer2D data = getData(filename);
    struct dataContainer1D returnedValue; 

    if (data.error) {
        returnedValue.error = 1;
        freeMalloc2D(data);
        return returnedValue;
    }

    returnedValue = getFieldValues(data, field);

    freeMalloc2D(data); 
    return returnedValue;
}

/*Horizontal search using field_name, return a horizontal line of values using the field
    * - example: 
        -- fieldname = Tags
        -- fieldvalue = Doctor
        returns in .data: 
                        * - POTATO1;password123;John Smith;Doctor;
                        * - POTATO4;abc123;Sarah Brown;Doctor;
                        * - POTATO7;secure432;Brian Miller;Doctor;
To call this function:
    struct dataContainer1D <container_name> = queryFieldStrict(file_name, field, key );
    -- container_name: is the name of the struct
    -- file_name: name of the file that you want to read
    -- field: the field you want to use for the search
    -- key : the value you want to search in the field
*/
struct dataContainer2D queryFieldStrict(const char* filename, char* field, char* key) 
{ 
    struct dataContainer2D data = getData(filename);
    struct dataContainer2D returnedValue; 

    if (data.error) {
        returnedValue.error = 1;
        return returnedValue;
    }

    returnedValue = filterDataContainer(data, field, key);
    
    freeMalloc2D(data);
    return returnedValue;
}

/*__________________________Functions that you can call ends here___________________________________________________________________________________*/

///////////////////////////////////* DATA WRITE FUNCTIONS*///////////////////////////////////////////////////////////////////////////////////////////////

//take 2D array and concatenate into one line and write into file
int writeData(const char* filename, struct dataContainer2D array) 
{
    FILE* filePointer = filecheck(filename, "w");
    
    char line[256]; 
    
    /*Field names*/
    line[0] = '\0'; // Initialize the line buffer
    
    //concatenate into one line
    for (int i = 0; i < array.x; i++)
    {
        strcat(line, array.fields[i]); 
        strcat(line, ";");
    }
    strcat(line, "\n");

    //write field_names into file
    fputs(line, filePointer);
    
    /*Values*/
    for (int i=0; i < array.y; i++)
    {   
        // Clear line buffer
        line[0] = '\0';
        
        //concatenate into one line
        for (int j = 0; j < array.x; j++)
        {
            strcat(line, array.data[i][j]); 
            strcat(line, ";");
        }
        strcat(line, "\n");

        //write line into file
        fputs(line, filePointer);

    }

    fclose(filePointer);

    freeMalloc2D(array);

    return 0; // Return if Nothing Goes Wrong
}

/*____________________________3 Functions that you can call starts here___________________________________________________________________________________*/

/* This function write a new record(values) to a file.
    *Parameter: 
    * - filename: name of the file without .txt [example : "Users1", "Patient_IDs"]
    * - numInputs: number of inputs, count your number of values stored in your array
    * - inputs: array of inputs
    
    * Sample of Implementation:

    *1. Get your users input as UserID, UserPW, Name, Tags

    *2. Put your values into the array using:  
        
    * - const char* input[] = {UserID, UserPW, Name, Tags};

    *3. Call the function write_new_data_file("filename", number_of_values, name_of_array);
        
    * - writre new data__file("Users", 4, inputs);

    OUTPUT:
    inside the file a brand new lines with ';' will be addded in bottom line
*/
int write_new_data(const char* filename, int numInputs, char* inputs[]) 
{
    // Make a copy of the input array to avoid modifying the original data
    char* inputs_copy[numInputs];
    for (int i = 0; i < numInputs; ++i) 
    {
        inputs_copy[i] = strdup(inputs[i]);
    }

    // Remove newline characters from each input
    for (int i = 0; i < numInputs; ++i) 
    {
        inputs_copy[i][strcspn(inputs_copy[i], "\n")] = '\0';
    }

    // Check for semicolon in inputs
    for (int i = 0; i < numInputs; i++) 
    {
        if (strchr(inputs_copy[i], ';') != NULL) 
        {
            fprintf(stderr, "Error: semicolon found in inputs[%d]!\n", i);
            return 1;
        }
    }

    FILE* filePointer = filecheck(filename, "a");
   
    // Calculate the total length needed for the input string
    int total_len = 0;
    for (int i = 0; i < numInputs; ++i) 
    {
        total_len += snprintf(NULL, 0, "%s;", inputs_copy[i]);
    }
    total_len += 1; // Add 1 for the newline character

    char* line = malloc(total_len + 10); // Allocate memory for the line
    
    // Check for null pointer
    if (line == NULL) 
    { 
        printf("Error allocating memory!\n");
        fclose(filePointer);
        return 1;
    }

    // Construct the input string
    line[0] = '\0';
    for (int i = 0; i < numInputs; ++i) 
    {
        strcat(line, inputs_copy[i]); 
        strcat(line, ";");
    }
    strcat(line, "\n"); // Add a newline character at the end

    // Write the input string to the file
    fputs(line, filePointer);

    // Cleanup
    free(line);
    fclose(filePointer);
    return 0;
}

/*Update existing record in existing file

*Parameter: 
    * - filename: name of the file without .txt [example : users, Patient_IDs, Inventory]

    * - relaying_array: array that store all updated inputs
        -- if you do: struct dataContainer1D master = queryKey("users", UserID);
        data.data is your array

        >> you can read struct datacontainer1D above

updateData("file_name", master.data);

Sample of implementation:

*1. Get your unqiue_key input as Unique_key

*2. use: struct dataContainer1D master = queryKey("file_name", Unique_key);
    * - the data is stored in master.data 

*3. Modify accordingly by using different index of master.data[ your_index_here ]

*4. call: updateData("file_name_without.txt", master.data);

*/
int updateData(const char* filename, char** relaying_array)
{   
    //getData of the file
    struct dataContainer2D master = getData(filename);
    const int uniqueIDIndex = 0;

    //iterate vertically through data search for the unique key
    for (int i=0; i<master.y; i++) 
    {   
        //IF FOUND the unique
        if(!strncmp(master.data[i][uniqueIDIndex], relaying_array[uniqueIDIndex], 255))
        {
            //store the relaying array into master

            for (int j=0; j<master.x; j++) 
            {
                master.data[i][j] = strdup(relaying_array[j]);
            }
        }
    }    

    return writeData(filename, master);
}

/*Update file that dont have primary key
 
*Parameter: 
    * - filename: name of the file without .txt [example : users, Patient_IDs, Inventory]

    * - relaying_array: array that store all updated inputs
        -- if you do: struct dataContainer1D master = queryKey("users", UserID);
        data.data is your array

        >> you can read struct datacontainer1D above

    * - second_key: the second key you want to search
        -- data types : char* second_key;

    * - second_key_index: the index (an integer) of the second key
        -- data types : int second_key_index;

updateData("file_name", master.data);

Sample of implementation:

*1. Get your unqiue_key input as Unique_key

*2. use: struct dataContainer1D master = queryKey("file_name", Unique_key);
    * - the data is stored in master.data 

*3. Modify accordingly by using different index of master.data[ your_index_here ]

*4. Determine your 2nd key and the index of 2nd key in the txt file.

*4. call: updateData("file_name_without.txt", master.data, second_key, second_key_index);

*/
int update_non_primary_Data(const char* filename, char** relaying_array, char* second_key, int second_key_index) 
{   
    //getData of the file
    struct dataContainer2D master = getData(filename);
    const int uniqueIDIndex = 0;

    //iterate vertically through data search for the unique key
    for (int i=0; i<master.y; i++) 
    {   
        //IF FOUND the unique
        if(!strncmp(master.data[i][uniqueIDIndex], relaying_array[uniqueIDIndex], 255))
        {
            //store the relaying array into master
            
            if (!strncmp(master.data[i][second_key_index], second_key, 255))
            {
                for (int j=0; j<master.x; j++) 
                {
                    master.data[i][j] = strdup(relaying_array[j]);
                }
            }
        
        }
    }    

    return writeData(filename, master);
}

/*Delete existing record in existing file:

*Parameter:
    * - filename: name of the file without .txt
    * - unique_key: unique key of the record you want to delete

deleteKey("filename", unique_key);

Sample of implementation:

*1. Get your the unqiue_key of the lines of record you want to delete

*2. call: deleteKey("filename_without.txt", unique_key);
*/
int deleteKey(const char* filename, char* unique_key) 
{
    struct dataContainer2D master = getData(filename);
    
    master.y = master.y - 1;

    for (int i=0; i<master.y; i++) 
    {   
        //IF FOUND the unique
        if(!strncmp(master.data[i][0], unique_key, 255))
        {   
            //delete that record
            free(master.data[i]);
            for (int j=i; j<master.y; j++)
            {
                master.data[j] = master.data[j+1];
            }
            
        }
    }    

    return writeData(filename, master);
}

/*Delete existing record in existing file using two keys:

*Parameter:
    * - filename: name of the file without .txt
    * - first_key: first key of the record you want to delete ( example: username )
    * - second_key: the second key you want to search (example: date)
        -- data types : char* second_key;

    * - second_key_index: the index (an integer) of the second key 
        -- data types : int second_key_index;

Sample of implementation:
When using this function, you should be dealing with the file that dont have unqiue primary key.
Thus, 2 keys is needed to determine the single line you wanted to delete. 
In this case, you need to know the index of the second key in the txt file.

assumption: first_key has index [0] in the txt file

*1. Get your the first and second key of the lines of record you want to delete

*2. call: delete_non_primary_Key("filename_without.txt", first_key, second_key, second_key_index);
*/
int delete_non_primary_Key(const char* filename, char* first_key, char* second_key, int second_key_index) 
{
    struct dataContainer2D master = getData(filename);
    
    master.y = master.y - 1;

    for (int i=0; i<master.y; i++) 
    {   
        //IF FOUND the unique
        if(!strncmp(master.data[i][0], first_key, 255))
        {   
            if (!strncmp(master.data[i][second_key_index], second_key, 255))
            {
                //delete that record
                free(master.data[i]);
                for (int j=i; j<master.y; j++)
                {
                    master.data[j] = master.data[j+1];
                }
            }
                
        }
    }    

    return writeData(filename, master);
}
/*___________________________Functions that you can call ends here_________________________________________________________________________*/