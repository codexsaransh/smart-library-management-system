/*
 * ===========================================================================
 *  SMART LIBRARY MANAGEMENT SYSTEM USING DATA STRUCTURES AND ALGORITHMS IN C
 * ===========================================================================
 *  Subject  : Data Structures and Algorithms
 *  Course   : B.Tech Information Technology, Semester III (GGSIPU)
 *
 *  Core DSA concepts implemented in this program:
 *    1. Linked List  - books, students and issue records are stored in
 *                      singly linked lists created with malloc().
 *    2. Queue        - every book owns a linked FIFO queue (front/rear
 *                      pointers) holding the students waiting for it.
 *    3. Linear Search- books are searched by ID or title, students and issue
 *                      records are located by scanning their lists.
 *    4. Bubble Sort  - books are sorted by Book ID.
 *  Additional C concept: File Handling (text files) for permanent storage.
 *
 *  Compile : gcc library.c -o library
 *  Run     : ./library        (Windows: library.exe)
 * ===========================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* ------------------------------ Constants -------------------------------- */
#define MAX_TITLE     25      /* maximum characters in a book title        */
#define MAX_AUTHOR    18      /* maximum characters in an author name      */
#define MAX_CATEGORY  16      /* maximum characters in a category          */
#define MAX_NAME      20      /* maximum characters in a student name      */
#define MAX_COURSE    15      /* maximum characters in a course name       */
#define MAX_ID        99999   /* largest allowed Book ID / Student ID      */
#define MAX_COPIES    100     /* largest allowed number of copies          */
#define LINE_SIZE     256     /* size of the buffer used to read a line    */
#define BOOKS_FILE    "books.txt"
#define STUDENTS_FILE "students.txt"
#define ISSUED_FILE   "issued_books.txt"
#define RESERVE_FILE  "reservations.txt"
/* ------------------------------ Structures ------------------------------- */
/* One node of the reservation queue (a student waiting for a book). */
typedef struct Reservation {
    int studentId;
    int bookId;
    struct Reservation *next;
} Reservation;
/* A linked queue: items leave from the front and join at the rear. */
typedef struct Queue {
    Reservation *front;
    Reservation *rear;
    int count;
} Queue;
/* One node of the Book linked list. Each book has its own waiting queue. */
typedef struct Book {
    int id;
    char title[MAX_TITLE + 1];
    char author[MAX_AUTHOR + 1];
    char category[MAX_CATEGORY + 1];
    int totalCopies;
    int availableCopies;
    Queue waitList;            /* students waiting for this book (FIFO) */
    struct Book *next;
} Book;
/* One node of the Student linked list. */
typedef struct Student {
    int id;
    char name[MAX_NAME + 1];
    char course[MAX_COURSE + 1];
    struct Student *next;
} Student;
/* One node of the Issued Book linked list (one record per issued copy). */
typedef struct IssuedBook {
    int issueId;
    int studentId;
    int bookId;
    struct IssuedBook *next;
} IssuedBook;
/* ------------------------------ Global data ------------------------------ */
Book       *bookHead    = NULL;   /* head pointer of the book list    */
Student    *studentHead = NULL;   /* head pointer of the student list */
IssuedBook *issueHead   = NULL;   /* head pointer of the issued list  */
int nextIssueId = 1;              /* number given to the next issue   */
/* --------------------------- Function prototypes ------------------------- */
void freeAllMemory(void);
void saveAllData(int showMessage);
/* ========================================================================== */
/*                         INPUT HELPER FUNCTIONS                             */
/* ========================================================================== */
/* Called when the input stream ends (e.g. Ctrl+D): clean up and quit. */
void handleEndOfInput(void)
{
    printf("\nInput ended. Exiting the program.\n");
    saveAllData(0);
    freeAllMemory();
    exit(0);
}
/* Reads one line of text safely (no gets/scanf problems) and removes '\n'. */
void readLine(const char *prompt, char *buffer, int size)
{
    size_t length;
    int ch;
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) == NULL) {
        handleEndOfInput();
    }
    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else {
        /* the line was longer than the buffer: discard the remaining part */
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }
}
/* Removes leading and trailing spaces from a string (in place). */
void trimSpaces(char *text)
{
    int start = 0;
    int end = (int)strlen(text) - 1;
    int i;
    while (text[start] == ' ' || text[start] == '\t') {
        start++;
    }
    while (end >= start && (text[end] == ' ' || text[end] == '\t')) {
        end--;
    }
    for (i = 0; i <= end - start; i++) {
        text[i] = text[start + i];
    }
    text[end - start + 1] = '\0';
}
/* Repeats the question until a whole number between min and max is given. */
int readInt(const char *prompt, int min, int max)
{
    char line[LINE_SIZE];
    char *endPointer;
    long value;
    while (1) {
        readLine(prompt, line, LINE_SIZE);
        value = strtol(line, &endPointer, 10);
        if (line[0] != '\0' && *endPointer == '\0' &&
            value >= min && value <= max) {
            return (int)value;
        }
        printf("Invalid input! Please enter a whole number between %d and %d.\n",
               min, max);
    }
}
/*
 * Reads a non-empty text value of at most maxLength characters.
 * The '|' character is not allowed because it separates fields in the files.
 * If allowEmpty is 1, an empty answer is accepted and 0 is returned.
 * Returns 1 when a new value was stored in 'result'.
 */
int readText(const char *prompt, char *result, int maxLength, int allowEmpty)
{
    char line[LINE_SIZE];
    while (1) {
        readLine(prompt, line, LINE_SIZE);
        trimSpaces(line);
        if (line[0] == '\0') {
            if (allowEmpty) {
                return 0;
            }
            printf("Input cannot be empty. Please try again.\n");
        } else if (strchr(line, '|') != NULL) {
            printf("The '|' character is not allowed. Please try again.\n");
        } else if ((int)strlen(line) > maxLength) {
            printf("Input too long (maximum %d characters). Please try again.\n",
                   maxLength);
        } else {
            strcpy(result, line);
            return 1;
        }
    }
}
/* Asks a yes/no question. Returns 1 for yes and 0 for no. */
int readYesNo(const char *prompt)
{
    char line[LINE_SIZE];
    while (1) {
        readLine(prompt, line, LINE_SIZE);
        trimSpaces(line);
        if (strcmp(line, "y") == 0 || strcmp(line, "Y") == 0) {
            return 1;
        }
        if (strcmp(line, "n") == 0 || strcmp(line, "N") == 0) {
            return 0;
        }
        printf("Please enter Y or N.\n");
    }
}
/* Converts one upper-case letter to lower case (used for title search). */
char toLowerChar(char ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return (char)(ch + ('a' - 'A'));
    }
    return ch;
}
/* Returns 1 if 'pattern' occurs inside 'text' (ignoring letter case). */
int containsIgnoreCase(const char *text, const char *pattern)
{
    int textLength = (int)strlen(text);
    int patternLength = (int)strlen(pattern);
    int i, j;
    if (patternLength == 0) {
        return 0;
    }
    for (i = 0; i + patternLength <= textLength; i++) {
        for (j = 0; j < patternLength; j++) {
            if (toLowerChar(text[i + j]) != toLowerChar(pattern[j])) {
                break;
            }
        }
        if (j == patternLength) {
            return 1;
        }
    }
    return 0;
}
/* ========================================================================== */
/*                          QUEUE OPERATIONS                                  */
/* ========================================================================== */
void initQueue(Queue *queue)
{
    queue->front = NULL;
    queue->rear = NULL;
    queue->count = 0;
}
int isQueueEmpty(const Queue *queue)
{
    return queue->front == NULL;
}
/* ENQUEUE: add a reservation at the rear of the queue. Returns 1 on success. */
int enqueue(Queue *queue, int studentId, int bookId)
{
    Reservation *node = (Reservation *)malloc(sizeof(Reservation));
    if (node == NULL) {
        printf("Error: memory allocation failed.\n");
        return 0;
    }
    node->studentId = studentId;
    node->bookId = bookId;
    node->next = NULL;
    if (queue->rear == NULL) {          /* queue was empty */
        queue->front = node;
    } else {
        queue->rear->next = node;
    }
    queue->rear = node;
    queue->count++;
    return 1;
}
/* DEQUEUE: remove the reservation at the front. Returns 0 if queue is empty. */
int dequeue(Queue *queue, int *studentId)
{
    Reservation *node;
    if (isQueueEmpty(queue)) {
        return 0;
    }
    node = queue->front;
    *studentId = node->studentId;
    queue->front = node->next;
    if (queue->front == NULL) {         /* queue became empty */
        queue->rear = NULL;
    }
    free(node);
    queue->count--;
    return 1;
}
/* Returns 1 if the student is already waiting in this queue. */
int queueContains(const Queue *queue, int studentId)
{
    const Reservation *current = queue->front;
    while (current != NULL) {
        if (current->studentId == studentId) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}
/* Removes every node of a queue (used before freeing a book). */
void clearQueue(Queue *queue)
{
    int dummy;
    while (dequeue(queue, &dummy)) {
    }
}
/* ========================================================================== */
/*                  LINKED LIST OPERATIONS - STUDENTS / ISSUES                */
/* ========================================================================== */
/* LINEAR SEARCH: find a student by ID. Returns NULL if not found. */
Student *searchStudentById(int studentId)
{
    Student *current = studentHead;
    while (current != NULL) {
        if (current->id == studentId) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
/* Insert a new student node at the end of the student list. */
void insertStudent(Student *newStudent)
{
    Student *current;
    newStudent->next = NULL;
    if (studentHead == NULL) {
        studentHead = newStudent;
        return;
    }
    current = studentHead;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newStudent;
}
/* LINEAR SEARCH: find the issue record of one student for one book. */
IssuedBook *findIssueRecord(int studentId, int bookId)
{
    IssuedBook *current = issueHead;
    while (current != NULL) {
        if (current->studentId == studentId && current->bookId == bookId) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
/* Insert a new issue record at the end of the issued-books list. */
int addIssueRecord(int studentId, int bookId)
{
    IssuedBook *node = (IssuedBook *)malloc(sizeof(IssuedBook));
    IssuedBook *current;
    if (node == NULL) {
        printf("Error: memory allocation failed.\n");
        return 0;
    }
    node->issueId = nextIssueId++;
    node->studentId = studentId;
    node->bookId = bookId;
    node->next = NULL;
    if (issueHead == NULL) {
        issueHead = node;
    } else {
        current = issueHead;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = node;
    }
    return 1;
}
/* Delete one issue record from the list (deletion from a linked list). */
void removeIssueRecord(IssuedBook *target)
{
    IssuedBook *current = issueHead;
    IssuedBook *previous = NULL;
    while (current != NULL && current != target) {
        previous = current;
        current = current->next;
    }
    if (current == NULL) {
        return;
    }
    if (previous == NULL) {
        issueHead = current->next;      /* deleting the first node */
    } else {
        previous->next = current->next;
    }
    free(current);
}
/* ========================================================================== */
/*                       LINKED LIST OPERATIONS - BOOKS                       */
/* ========================================================================== */
/* LINEAR SEARCH BY ID: returns the matching node or NULL. */
Book *searchBookById(int bookId)
{
    Book *current = bookHead;
    while (current != NULL) {
        if (current->id == bookId) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
/* Insert a new book node at the end of the book list. */
void insertBook(Book *newBook)
{
    Book *current;
    newBook->next = NULL;
    if (bookHead == NULL) {
        bookHead = newBook;
        return;
    }
    current = bookHead;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newBook;
}
/* Delete a book node (and free its memory) from the book list. */
void deleteBookNode(Book *target)
{
    Book *current = bookHead;
    Book *previous = NULL;
    while (current != NULL && current != target) {
        previous = current;
        current = current->next;
    }
    if (current == NULL) {
        return;
    }
    if (previous == NULL) {
        bookHead = current->next;
    } else {
        previous->next = current->next;
    }
    clearQueue(&current->waitList);
    free(current);
}
/* Prints the heading line of the book table. */
void printBookHeader(void)
{
    printf("%-6s %-25s %-18s %-16s %5s %5s\n",
           "BookID", "Title", "Author", "Category", "Total", "Avail");
    printf("--------------------------------------------------------"
           "------------------------\n");
}
/* Prints one book as a row of the table. */
void printBookRow(const Book *book)
{
    printf("%-6d %-25s %-18s %-16s %5d %5d\n",
           book->id, book->title, book->author, book->category,
           book->totalCopies, book->availableCopies);
}
/* Prints the whole book list (TRAVERSAL of the linked list). */
void printAllBooks(void)
{
    Book *current = bookHead;
    printBookHeader();
    while (current != NULL) {
        printBookRow(current);
        current = current->next;
    }
}
/* Counts the nodes in the book list. */
int countBooks(void)
{
    int count = 0;
    Book *current = bookHead;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}
/* ========================================================================== */
/*                  RESERVATION PROCESSING (uses DEQUEUE)                     */
/* ========================================================================== */
/*
 * If copies of 'book' are available and students are waiting, each copy is
 * given to the student at the FRONT of this book's queue (FIFO order).
 * Because every book has its own queue, a student can never receive a book
 * other than the one he/she reserved.
 */
void processReservations(Book *book)
{
    int studentId;
    Student *student;
    while (book->availableCopies > 0 && !isQueueEmpty(&book->waitList)) {
        dequeue(&book->waitList, &studentId);
        if (addIssueRecord(studentId, book->id)) {
            book->availableCopies--;
            student = searchStudentById(studentId);
            printf("Reservation processed: Student %d (%s) was first in the "
                   "waiting list\n"
                   "and has been issued Book %d (%s).\n",
                   studentId, student != NULL ? student->name : "unknown",
                   book->id, book->title);
        }
    }
}
/* ========================================================================== */
/*                                FILE HANDLING                               */
/* ========================================================================== */
/* Replaces the real file with the temporary file that was just written. */
void replaceFile(const char *tempName, const char *realName)
{
    remove(realName);                   /* needed on Windows */
    if (rename(tempName, realName) != 0) {
        printf("Error: could not update %s.\n", realName);
    }
}
/*
 * Each save function first writes to a temporary file. The real file is
 * replaced only after the new copy was written completely, so a failure while
 * writing cannot destroy the old data.
 */
void saveBooks(void)
{
    FILE *fp = fopen("books.tmp", "w");
    Book *current;
    if (fp == NULL) {
        printf("Error: could not write %s.\n", BOOKS_FILE);
        return;
    }
    for (current = bookHead; current != NULL; current = current->next) {
        fprintf(fp, "%d|%s|%s|%s|%d|%d\n", current->id, current->title,
                current->author, current->category,
                current->totalCopies, current->availableCopies);
    }
    fclose(fp);
    replaceFile("books.tmp", BOOKS_FILE);
}
void saveStudents(void)
{
    FILE *fp = fopen("students.tmp", "w");
    Student *current;
    if (fp == NULL) {
        printf("Error: could not write %s.\n", STUDENTS_FILE);
        return;
    }
    for (current = studentHead; current != NULL; current = current->next) {
        fprintf(fp, "%d|%s|%s\n", current->id, current->name, current->course);
    }
    fclose(fp);
    replaceFile("students.tmp", STUDENTS_FILE);
}
void saveIssuedBooks(void)
{
    FILE *fp = fopen("issued.tmp", "w");
    IssuedBook *current;
    if (fp == NULL) {
        printf("Error: could not write %s.\n", ISSUED_FILE);
        return;
    }
    for (current = issueHead; current != NULL; current = current->next) {
        fprintf(fp, "%d|%d|%d\n", current->issueId, current->studentId,
                current->bookId);
    }
    fclose(fp);
    replaceFile("issued.tmp", ISSUED_FILE);
}
/* Reservations are written book by book, front of the queue first. */
void saveReservations(void)
{
    FILE *fp = fopen("reserve.tmp", "w");
    Book *book;
    Reservation *node;
    if (fp == NULL) {
        printf("Error: could not write %s.\n", RESERVE_FILE);
        return;
    }
    for (book = bookHead; book != NULL; book = book->next) {
        for (node = book->waitList.front; node != NULL; node = node->next) {
            fprintf(fp, "%d|%d\n", node->bookId, node->studentId);
        }
    }
    fclose(fp);
    replaceFile("reserve.tmp", RESERVE_FILE);
}
/* Saves all four files. */
void saveAllData(int showMessage)
{
    saveBooks();
    saveStudents();
    saveIssuedBooks();
    saveReservations();
    if (showMessage) {
        printf("(Data saved to books.txt, students.txt, issued_books.txt "
               "and reservations.txt)\n");
    }
}
void loadBooks(void)
{
    FILE *fp = fopen(BOOKS_FILE, "r");
    int id, total, available, count = 0;
    char title[MAX_TITLE + 1], author[MAX_AUTHOR + 1], category[MAX_CATEGORY + 1];
    Book *book;
    if (fp == NULL) {
        printf("%s not found. Starting with empty book records.\n", BOOKS_FILE);
        return;
    }
    /* field widths below match MAX_TITLE, MAX_AUTHOR and MAX_CATEGORY */
    while (fscanf(fp, "%d|%25[^|]|%18[^|]|%16[^|]|%d|%d\n",
                  &id, title, author, category, &total, &available) == 6) {
        if (id < 1 || total < 1 || available < 0 || available > total ||
            searchBookById(id) != NULL) {
            continue;                   /* skip an invalid or duplicate record */
        }
        book = (Book *)malloc(sizeof(Book));
        if (book == NULL) {
            break;
        }
        book->id = id;
        strcpy(book->title, title);
        strcpy(book->author, author);
        strcpy(book->category, category);
        book->totalCopies = total;
        book->availableCopies = available;
        initQueue(&book->waitList);
        insertBook(book);
        count++;
    }
    fclose(fp);
    printf("%d book record(s) loaded from %s.\n", count, BOOKS_FILE);
}
void loadStudents(void)
{
    FILE *fp = fopen(STUDENTS_FILE, "r");
    int id, count = 0;
    char name[MAX_NAME + 1], course[MAX_COURSE + 1];
    Student *student;
    if (fp == NULL) {
        printf("%s not found. Starting with empty student records.\n",
               STUDENTS_FILE);
        return;
    }
    while (fscanf(fp, "%d|%20[^|]|%15[^|\n]\n", &id, name, course) == 3) {
        if (id < 1 || searchStudentById(id) != NULL) {
            continue;
        }
        student = (Student *)malloc(sizeof(Student));
        if (student == NULL) {
            break;
        }
        student->id = id;
        strcpy(student->name, name);
        strcpy(student->course, course);
        insertStudent(student);
        count++;
    }
    fclose(fp);
    printf("%d student record(s) loaded from %s.\n", count, STUDENTS_FILE);
}
void loadIssuedBooks(void)
{
    FILE *fp = fopen(ISSUED_FILE, "r");
    int issueId, studentId, bookId, count = 0;
    IssuedBook *node, *current;
    if (fp == NULL) {
        printf("%s not found. Starting with empty issue records.\n", ISSUED_FILE);
        return;
    }
    while (fscanf(fp, "%d|%d|%d\n", &issueId, &studentId, &bookId) == 3) {
        node = (IssuedBook *)malloc(sizeof(IssuedBook));
        if (node == NULL) {
            break;
        }
        node->issueId = issueId;
        node->studentId = studentId;
        node->bookId = bookId;
        node->next = NULL;
        if (issueHead == NULL) {
            issueHead = node;
        } else {
            current = issueHead;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = node;
        }
        if (issueId >= nextIssueId) {
            nextIssueId = issueId + 1;  /* continue numbering after the last */
        }
        count++;
    }
    fclose(fp);
    printf("%d issue record(s) loaded from %s.\n", count, ISSUED_FILE);
}
/* Rebuilds each book's waiting queue; file order = FIFO order. */
void loadReservations(void)
{
    FILE *fp = fopen(RESERVE_FILE, "r");
    int bookId, studentId, count = 0;
    Book *book;
    if (fp == NULL) {
        printf("%s not found. Starting with empty reservation records.\n",
               RESERVE_FILE);
        return;
    }
    while (fscanf(fp, "%d|%d\n", &bookId, &studentId) == 2) {
        book = searchBookById(bookId);
        if (book != NULL && enqueue(&book->waitList, studentId, bookId)) {
            count++;
        }
    }
    fclose(fp);
    printf("%d reservation record(s) loaded from %s.\n", count, RESERVE_FILE);
}
void loadAllData(void)
{
    loadBooks();
    loadStudents();
    loadIssuedBooks();
    loadReservations();
}
/* ========================================================================== */
/*                               MENU MODULES                                 */
/* ========================================================================== */
/* Module 1: Add New Book (linked list insertion) */
void addBook(void)
{
    Book *book;
    int id, quantity;
    printf("\n--- ADD NEW BOOK ---\n");
    id = readInt("Enter Book ID (1-99999): ", 1, MAX_ID);
    if (searchBookById(id) != NULL) {
        printf("Error: Book ID %d already exists. "
               "Duplicate IDs are not allowed.\n", id);
        return;
    }
    book = (Book *)malloc(sizeof(Book));
    if (book == NULL) {
        printf("Error: memory allocation failed.\n");
        return;
    }
    book->id = id;
    readText("Enter Title: ", book->title, MAX_TITLE, 0);
    readText("Enter Author: ", book->author, MAX_AUTHOR, 0);
    readText("Enter Category: ", book->category, MAX_CATEGORY, 0);
    quantity = readInt("Enter Quantity (1-100): ", 1, MAX_COPIES);
    book->totalCopies = quantity;
    book->availableCopies = quantity;
    initQueue(&book->waitList);
    insertBook(book);
    printf("Book added successfully.\n");
    saveAllData(1);
}
/* Module 2: Display All Books (linked list traversal) */
void displayBooks(void)
{
    printf("\n--- ALL BOOKS ---\n");
    if (bookHead == NULL) {
        printf("No books available in the library.\n");
        return;
    }
    printAllBooks();
    printf("Total book records: %d\n", countBooks());
}
/* Module 3: Search Book (LINEAR SEARCH by ID or by title) */
void searchBook(void)
{
    int choice, id, found;
    char title[MAX_TITLE + 1];
    Book *current;
    printf("\n--- SEARCH BOOK ---\n");
    printf("1. Search by Book ID\n");
    printf("2. Search by Title\n");
    choice = readInt("Enter your choice (1-2): ", 1, 2);
    if (bookHead == NULL) {
        printf("No books available in the library.\n");
        return;
    }
    if (choice == 1) {
        id = readInt("Enter Book ID to search: ", 1, MAX_ID);
        current = searchBookById(id);          /* linear search */
        if (current == NULL) {
            printf("Book with ID %d not found.\n", id);
            return;
        }
        printf("Book found:\n");
        printBookHeader();
        printBookRow(current);
    } else {
        readText("Enter Title (or part of the title): ", title, MAX_TITLE, 0);
        found = 0;
        current = bookHead;
        while (current != NULL) {              /* linear search */
            if (containsIgnoreCase(current->title, title)) {
                if (found == 0) {
                    printf("Matching book(s):\n");
                    printBookHeader();
                }
                printBookRow(current);
                found++;
            }
            current = current->next;
        }
        if (found == 0) {
            printf("No book found with title containing \"%s\".\n", title);
        }
    }
}
/* Module 4: Issue Book */
void issueBook(void)
{
    int studentId, bookId;
    Student *student;
    Book *book;
    printf("\n--- ISSUE BOOK ---\n");
    studentId = readInt("Enter Student ID: ", 1, MAX_ID);
    student = searchStudentById(studentId);
    if (student == NULL) {
        printf("Error: Student ID %d is not registered.\n", studentId);
        return;
    }
    bookId = readInt("Enter Book ID: ", 1, MAX_ID);
    book = searchBookById(bookId);
    if (book == NULL) {
        printf("Error: Book ID %d not found.\n", bookId);
        return;
    }
    if (findIssueRecord(studentId, bookId) != NULL) {
        printf("Error: %s already has a copy of this book.\n", student->name);
        return;
    }
    if (book->availableCopies == 0) {
        printf("Book \"%s\" is currently unavailable (all %d copies are issued).\n",
               book->title, book->totalCopies);
        printf("Please use option 6 (Reserve Book) to join the waiting list.\n");
        return;
    }
    if (!addIssueRecord(studentId, bookId)) {
        return;
    }
    book->availableCopies--;
    printf("Book \"%s\" issued to %s (Student ID %d).\n",
           book->title, student->name, studentId);
    printf("Copies available now: %d\n", book->availableCopies);
    saveAllData(1);
}
/* Module 5: Return Book (then DEQUEUE the next waiting student, if any) */
void returnBook(void)
{
    int studentId, bookId;
    IssuedBook *record;
    Book *book;
    Student *student;
    printf("\n--- RETURN BOOK ---\n");
    studentId = readInt("Enter Student ID: ", 1, MAX_ID);
    bookId = readInt("Enter Book ID: ", 1, MAX_ID);
    record = findIssueRecord(studentId, bookId);
    if (record == NULL) {
        printf("Error: No record found of Book ID %d issued to Student ID %d.\n",
               bookId, studentId);
        return;
    }
    book = searchBookById(bookId);
    student = searchStudentById(studentId);
    removeIssueRecord(record);
    if (book != NULL) {
        book->availableCopies++;
        printf("Book \"%s\" returned by %s (Student ID %d).\n", book->title,
               student != NULL ? student->name : "unknown", studentId);
        printf("Copies available now: %d\n", book->availableCopies);
        if (isQueueEmpty(&book->waitList)) {
            printf("No students are waiting for this book.\n");
        } else {
            printf("Checking the reservation waiting list...\n");
            processReservations(book);
        }
    }
    saveAllData(1);
}
/* Module 6: Reserve Book (ENQUEUE) */
void reserveBook(void)
{
    int studentId, bookId;
    Student *student;
    Book *book;
    printf("\n--- RESERVE BOOK ---\n");
    studentId = readInt("Enter Student ID: ", 1, MAX_ID);
    student = searchStudentById(studentId);
    if (student == NULL) {
        printf("Error: Student ID %d is not registered.\n", studentId);
        return;
    }
    bookId = readInt("Enter Book ID: ", 1, MAX_ID);
    book = searchBookById(bookId);
    if (book == NULL) {
        printf("Error: Book ID %d not found.\n", bookId);
        return;
    }
    if (book->availableCopies > 0) {
        printf("Book \"%s\" is available (%d copies). Please issue it directly.\n",
               book->title, book->availableCopies);
        return;
    }
    if (findIssueRecord(studentId, bookId) != NULL) {
        printf("Error: %s already has a copy of this book.\n", student->name);
        return;
    }
    if (queueContains(&book->waitList, studentId)) {
        printf("Error: %s is already in the waiting list for this book.\n",
               student->name);
        return;
    }
    if (enqueue(&book->waitList, studentId, bookId)) {
        printf("Reservation added. %s is at position %d in the\n"
               "waiting list for \"%s\".\n",
               student->name, book->waitList.count, book->title);
        saveAllData(1);
    }
}
/* Module 7: Add Student */
void addStudent(void)
{
    Student *student;
    int id;
    printf("\n--- ADD STUDENT ---\n");
    id = readInt("Enter Student ID (1-99999): ", 1, MAX_ID);
    if (searchStudentById(id) != NULL) {
        printf("Error: Student ID %d already exists. "
               "Duplicate IDs are not allowed.\n", id);
        return;
    }
    student = (Student *)malloc(sizeof(Student));
    if (student == NULL) {
        printf("Error: memory allocation failed.\n");
        return;
    }
    student->id = id;
    readText("Enter Student Name: ", student->name, MAX_NAME, 0);
    readText("Enter Course: ", student->course, MAX_COURSE, 0);
    insertStudent(student);
    printf("Student registered successfully.\n");
    saveAllData(1);
}
/* Module 8: Display Student Records */
void displayStudents(void)
{
    Student *current = studentHead;
    int count = 0;
    printf("\n--- STUDENT RECORDS ---\n");
    if (current == NULL) {
        printf("No students registered.\n");
        return;
    }
    printf("%-10s %-20s %-15s\n", "StudentID", "Name", "Course");
    printf("------------------------------------------\n");
    while (current != NULL) {
        printf("%-10d %-20s %-15s\n", current->id, current->name, current->course);
        count++;
        current = current->next;
    }
    printf("Total students: %d\n", count);
}
/* Module 9: Sort Books by ID (BUBBLE SORT on the linked list) */
void sortBooksById(void)
{
    Book *current, *nextNode;
    Book swapHolder;
    Book *savedCurrentNext, *savedNextNext;
    int swapped, pass = 0;
    printf("\n--- SORT BOOKS BY ID (BUBBLE SORT) ---\n");
    if (bookHead == NULL) {
        printf("No books available to sort.\n");
        return;
    }
    printf("Before sorting:\n");
    printAllBooks();
    do {
        swapped = 0;
        current = bookHead;
        while (current->next != NULL) {
            nextNode = current->next;
            if (current->id > nextNode->id) {
                /* swap the DATA of the two nodes; the links stay unchanged */
                swapHolder = *current;
                savedCurrentNext = current->next;
                savedNextNext = nextNode->next;
                *current = *nextNode;
                *nextNode = swapHolder;
                current->next = savedCurrentNext;
                nextNode->next = savedNextNext;
                swapped = 1;
            }
            current = current->next;
        }
        pass++;
    } while (swapped);
    printf("\nAfter sorting (ascending order of Book ID, %d pass(es)):\n", pass);
    printAllBooks();
    saveAllData(1);
}
/* Module 10: View Issued Books */
void viewIssuedBooks(void)
{
    IssuedBook *current = issueHead;
    Student *student;
    Book *book;
    int count = 0;
    printf("\n--- ISSUED BOOKS ---\n");
    if (current == NULL) {
        printf("No books are currently issued.\n");
        return;
    }
    printf("%-7s %-9s %-20s %-6s %-25s\n",
           "IssueNo", "StudentID", "Student Name", "BookID", "Title");
    printf("--------------------------------------------------------"
           "-----------\n");
    while (current != NULL) {
        student = searchStudentById(current->studentId);
        book = searchBookById(current->bookId);
        printf("%-7d %-9d %-20s %-6d %-25s\n", current->issueId,
               current->studentId, student != NULL ? student->name : "unknown",
               current->bookId, book != NULL ? book->title : "unknown");
        count++;
        current = current->next;
    }
    printf("Total issued books: %d\n", count);
}
/* Module 11: View Reservation Waiting List (queue traversal, front to rear) */
void viewWaitingList(void)
{
    Book *book;
    Reservation *node;
    Student *student;
    int position, anyWaiting = 0;
    printf("\n--- RESERVATION WAITING LIST ---\n");
    for (book = bookHead; book != NULL; book = book->next) {
        if (isQueueEmpty(&book->waitList)) {
            continue;
        }
        anyWaiting = 1;
        printf("Book %d - %s (%d waiting)\n", book->id, book->title,
               book->waitList.count);
        position = 1;
        for (node = book->waitList.front; node != NULL; node = node->next) {
            student = searchStudentById(node->studentId);
            printf("   %d. Student %d (%s)%s\n", position, node->studentId,
                   student != NULL ? student->name : "unknown",
                   position == 1 ? "  <- front of queue" : "");
            position++;
        }
    }
    if (!anyWaiting) {
        printf("The waiting list is empty. No reservations.\n");
    }
}
/* Extra module 13: Update Book Details */
void updateBook(void)
{
    int id, issuedCopies, newTotal;
    Book *book;
    char title[MAX_TITLE + 1], author[MAX_AUTHOR + 1], category[MAX_CATEGORY + 1];
    char prompt[LINE_SIZE];
    printf("\n--- UPDATE BOOK DETAILS ---\n");
    id = readInt("Enter Book ID to update: ", 1, MAX_ID);
    book = searchBookById(id);
    if (book == NULL) {
        printf("Book with ID %d not found.\n", id);
        return;
    }
    printf("Current details:\n");
    printBookHeader();
    printBookRow(book);
    printf("(Press Enter to keep the current value.)\n");
    sprintf(prompt, "New Title [%s]: ", book->title);
    if (readText(prompt, title, MAX_TITLE, 1)) {
        strcpy(book->title, title);
    }
    sprintf(prompt, "New Author [%s]: ", book->author);
    if (readText(prompt, author, MAX_AUTHOR, 1)) {
        strcpy(book->author, author);
    }
    sprintf(prompt, "New Category [%s]: ", book->category);
    if (readText(prompt, category, MAX_CATEGORY, 1)) {
        strcpy(book->category, category);
    }
    /* total copies can never be less than the copies that are issued now */
    issuedCopies = book->totalCopies - book->availableCopies;
    sprintf(prompt, "New Total Copies (current %d, minimum %d): ",
            book->totalCopies, issuedCopies > 1 ? issuedCopies : 1);
    newTotal = readInt(prompt, issuedCopies > 1 ? issuedCopies : 1, MAX_COPIES);
    book->totalCopies = newTotal;
    book->availableCopies = newTotal - issuedCopies;
    printf("Book details updated successfully.\n");
    processReservations(book);      /* new copies may serve waiting students */
    saveAllData(1);
}
/* Extra module 14: Delete Book (linked list deletion) */
void deleteBook(void)
{
    int id;
    Book *book;
    printf("\n--- DELETE BOOK ---\n");
    id = readInt("Enter Book ID to delete: ", 1, MAX_ID);
    book = searchBookById(id);
    if (book == NULL) {
        printf("Book with ID %d not found.\n", id);
        return;
    }
    if (book->availableCopies != book->totalCopies) {
        printf("Cannot delete \"%s\": %d copy(ies) are currently issued.\n",
               book->title, book->totalCopies - book->availableCopies);
        return;
    }
    if (!readYesNo("Are you sure you want to delete this book? (Y/N): ")) {
        printf("Deletion cancelled.\n");
        return;
    }
    deleteBookNode(book);
    printf("Book deleted successfully.\n");
    saveAllData(1);
}
/* ========================================================================== */
/*                       MEMORY CLEAN-UP AND MAIN MENU                        */
/* ========================================================================== */
/* Frees every dynamically allocated node before the program ends. */
void freeAllMemory(void)
{
    Book *book;
    Student *student;
    IssuedBook *issue;
    while (bookHead != NULL) {
        book = bookHead;
        bookHead = bookHead->next;
        clearQueue(&book->waitList);
        free(book);
    }
    while (studentHead != NULL) {
        student = studentHead;
        studentHead = studentHead->next;
        free(student);
    }
    while (issueHead != NULL) {
        issue = issueHead;
        issueHead = issueHead->next;
        free(issue);
    }
}
void showMenu(void)
{
    printf("\n==========================================\n");
    printf("     SMART LIBRARY MANAGEMENT SYSTEM\n");
    printf("==========================================\n");
    printf(" 1. Add New Book\n");
    printf(" 2. Display All Books\n");
    printf(" 3. Search Book\n");
    printf(" 4. Issue Book\n");
    printf(" 5. Return Book\n");
    printf(" 6. Reserve Book\n");
    printf(" 7. Add Student\n");
    printf(" 8. Display Student Records\n");
    printf(" 9. Sort Books by ID\n");
    printf("10. View Issued Books\n");
    printf("11. View Reservation Waiting List\n");
    printf("12. Exit\n");
    printf("13. Update Book Details\n");
    printf("14. Delete Book\n");
    printf("------------------------------------------\n");
}
int main(void)
{
    int choice;
    printf("Loading saved records...\n");
    loadAllData();
    do {
        showMenu();
        choice = readInt("Enter your choice (1-14): ", 1, 14);
        switch (choice) {
            case 1:  addBook();          break;
            case 2:  displayBooks();     break;
            case 3:  searchBook();       break;
            case 4:  issueBook();        break;
            case 5:  returnBook();       break;
            case 6:  reserveBook();      break;
            case 7:  addStudent();       break;
            case 8:  displayStudents();  break;
            case 9:  sortBooksById();    break;
            case 10: viewIssuedBooks();  break;
            case 11: viewWaitingList();  break;
            case 12:
                saveAllData(0);
                freeAllMemory();
                printf("\nAll records saved. Memory released.\n"
                       "Thank you for using the Smart Library "
                       "Management System!\n");
                break;
            case 13: updateBook();       break;
            case 14: deleteBook();       break;
        }
    } while (choice != 12);
    return 0;
}