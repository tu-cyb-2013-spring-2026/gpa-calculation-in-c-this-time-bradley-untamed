#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Structure to store individual course data
typedef struct {
    char grade;
    int hours;
    char semester[10];
    int year;
} Course;

// Helper: Checks if a string contains only digits
int is_numeric(const char *str) {
    if (*str == '\0') return 0;
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}


// Function to convert letter grade to 4.0 scale points
float get_grade_points(char grade) {
    switch (toupper(grade)) {
        case 'A': return 4.0;
        case 'B': return 3.0;
        case 'C': return 2.0;
        case 'D': return 1.0;
        case 'F': return 0.0;
        default:  return 0.0;
    }
}


int main()
{
    Course *courses = NULL;
    int count = 0;
    char input[100];
    
    printf("----> To quit entering grades, enter a blank line.\n");

    while (1) {
        // Requesting input from the user
        printf("Enter a letter grade and hours for a course with its year and semester: ");
        if (fgets(input, sizeof(input), stdin) == NULL || input[0] == '\n') {
            break;
        }

        // Expand memory for a new course entry
        Course *temp = realloc(courses, (count + 1) * sizeof(Course));
        if (temp == NULL) {
            printf("Memory error!\n");
            return 1;
        }

        courses = temp;

        // Validation Flags
        int v_grade = 0, v_hours = 0, v_sem = 0, v_year = 0;

        // Tokenize the input (Split by spaces, tabs, or newlines) and Validate
        char *token = strtok(input, " \t\n");
        while (token != NULL) {
            int len = strlen(token);

            if (is_numeric(token)) {
                int val = atoi(token);
                // Heuristic: If it's > 100, it's likely a year (like 2025)
                if (val >= 1900 && val <= 2100) {
                    courses[count].year = val;
                    v_year = 1;
                } else if (val >= 1 && val <= 4) {
                    courses[count].hours = val;
                    v_hours = 1;
                }
            } 
            else if (len == 1 && isalpha(token[0])) {
                char g = toupper(token[0]);
                if (strchr("ABCDF", g)) { // Efficient check for A,B,C,D,F
                    courses[count].grade = g;
                    v_grade = 1;
                }
            } 
            else {
                char s[10];
                strncpy(s, token, 9);
                for(int i=0; s[i]; i++) s[i] = toupper(s[i]);
                
                if (strcmp(s, "FALL") == 0 || strcmp(s, "SPRING") == 0 || strcmp(s, "SUMMER") == 0) {
                    strcpy(courses[count].semester, s);
                    v_sem = 1;
                }
            }
            token = strtok(NULL, " \t\n");
        }
        // The "Decision" Point
        if (v_grade && v_hours && v_sem && v_year) {
            count++; // Successfully saved!
            printf("   [Added successfully. Total courses: %d]\n", count);
        } else {
            printf("!! Error: Missing or invalid info. Course not saved.\n");
            // By NOT incrementing count, the next successful entry 
            // will just overwrite this "failed" slot.
        }
    }

    // --- TRANSCRIPT GENERATION ---
    if (count == 0) return 0;

    printf("\n-------------------------------------\n");
    printf("Unofficial Transcript\n");

    int *seen = calloc(count, sizeof(int));
    float grand_points = 0;
    int grand_hours = 0;

    for (int i = 0; i < count; i++) {
        if (seen[i]) continue;

        float sem_points = 0;
        int sem_hours = 0;

        // Grouping logic: Find all courses matching this semester and year
        for (int j = i; j < count; j++) {
            if (!seen[j] && courses[i].year == courses[j].year && 
                strcmp(courses[i].semester, courses[j].semester) == 0) {
                
                sem_points += (get_grade_points(courses[j].grade) * courses[j].hours);
                sem_hours += courses[j].hours;
                seen[j] = 1;
            }
        }

        printf("%-7s %d (%d hours) %.3f\n", 
               courses[i].semester, courses[i].year, sem_hours, sem_points / (sem_hours ? sem_hours : 1));
        
        grand_points += sem_points;
        grand_hours += sem_hours;
    }

    printf("\n--\nTotal credit hours: %d\n", grand_hours);
    printf("Overall GPA: %.3f\n-------------------------------------\n", grand_points / (grand_hours ? grand_hours : 1));

    free(courses);
    free(seen);
    return 0;
}
