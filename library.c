/*
 * SMART LIBRARY MANAGEMENT SYSTEM USING DATA STRUCTURES IN C
 * Subject : Data Structures
 * Course  : B.Tech Information Technology, Semester III 
 *
 * Concepts used:
 *  1. Linked List  - books, students and issue records (malloc/free)
 *  2. Queue    	- one linked FIFO queue of waiting students per book
 *  3. Linear Search- search by Book ID / title, find student / issue record
 *  4. Bubble Sort  - sort books by Book ID
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
/* ---------- Structures ---------- */
typedef struct Reservation {      	/* node of a queue */
    int studentId;
    struct Reservation *next;
} Reservation;
 
typedef struct {                  	/* queue with front and rear */
    Reservation *front;
    Reservation *rear;
} Queue;
 
typedef struct Book {             	/* node of the book list */
    int id;
    char title[30];
    char author[20];
    int totalCopies;
    int availableCopies;
    Queue waitList;                   /* students waiting for this book */
    struct Book *next;
} Book;
 
typedef struct Student {          	/* node of the student list */
    int id;
    char name[20];
    struct Student *next;
} Student;
 
typedef struct Issue {            	/* node of the issued-book list */
    int studentId;
    int bookId;
    struct Issue *next;
} Issue;
 
Book *bookHead = NULL;
Student *studentHead = NULL;
Issue *issueHead = NULL;
 
/* ---------- Queue operations ---------- */
void initQueue(Queue *q) { q->front = q->rear = NULL; }
 
int isEmpty(Queue *q) { return q->front == NULL; }
 
void enqueue(Queue *q, int studentId)       	/* insert at rear */
{
    Reservation *n = (Reservation *)malloc(sizeof(Reservation));
    n->studentId = studentId;
    n->next = NULL;
    if (q->rear == NULL) q->front = n;
    else q->rear->next = n;
    q->rear = n;
}
 
int dequeue(Queue *q)                       	/* remove from front */
{
    Reservation *n = q->front;
    int id = n->studentId;
    q->front = n->next;
    if (q->front == NULL) q->rear = NULL;
    free(n);
    return id;
}
 
int inQueue(Queue *q, int studentId)        	/* linear search in queue */
{
    Reservation *c;
    for (c = q->front; c != NULL; c = c->next)
        if (c->studentId == studentId) return 1;
    return 0;
}
 
/* ---------- Linear search ---------- */
Book *searchBookById(int id)
{
    Book *c = bookHead;
    while (c != NULL && c->id != id) c = c->next;
    return c;
}
 
Student *searchStudent(int id)
{
    Student *c = studentHead;
    while (c != NULL && c->id != id) c = c->next;
    return c;
}
 
Issue *searchIssue(int studentId, int bookId)
{
    Issue *c = issueHead;
    while (c != NULL && !(c->studentId == studentId && c->bookId == bookId))
        c = c->next;
    return c;
}
 
/* ---------- Linked list: insertion at end ---------- */
void insertBook(Book *n)
{
    Book *c;
    n->next = NULL;
    if (bookHead == NULL) { bookHead = n; return; }
    for (c = bookHead; c->next != NULL; c = c->next) ;
    c->next = n;
}
 
void insertStudent(Student *n)
{
    Student *c;
    n->next = NULL;
    if (studentHead == NULL) { studentHead = n; return; }
    for (c = studentHead; c->next != NULL; c = c->next) ;
    c->next = n;
}
 
void addIssue(int studentId, int bookId)
{
    Issue *n = (Issue *)malloc(sizeof(Issue));
    Issue *c;
    n->studentId = studentId;
    n->bookId = bookId;
    n->next = NULL;
    if (issueHead == NULL) { issueHead = n; return; }
    for (c = issueHead; c->next != NULL; c = c->next) ;
    c->next = n;
}
 
/* ---------- Linked list: deletion ---------- */
void removeIssue(Issue *target)
{
    Issue *c = issueHead, *prev = NULL;
    while (c != NULL && c != target) { prev = c; c = c->next; }
    if (c == NULL) return;
    if (prev == NULL) issueHead = c->next;
    else prev->next = c->next;
    free(c);
}
 
/* ---------- Helpers ---------- */
int readIntMin(const char *msg, int min)
{
    char line[64];
    int v;
    while (1) {
        printf("%s", msg);
        if (fgets(line, sizeof(line), stdin) == NULL) exit(0);
        if (sscanf(line, "%d", &v) == 1 && v >= min) return v;
        printf("Invalid input. Enter a valid number.\n");
    }
}
 
int readInt(const char *msg) { return readIntMin(msg, 1); }
 
void readText(const char *msg, char *dest, int size)
{
    printf("%s", msg);
    if (fgets(dest, size, stdin) == NULL) exit(0);
    dest[strcspn(dest, "\n")] = '\0';
}
 
void toLowerStr(const char *s, char *out)
{
    int i;
    for (i = 0; s[i]; i++)
        out[i] = (s[i] >= 'A' && s[i] <= 'Z') ? s[i] + 32 : s[i];
    out[i] = '\0';
}
 
void printBook(Book *b)
{
    printf("%-6d %-24s %-16s %-6d %-6d\n",
           b->id, b->title, b->author, b->totalCopies, b->availableCopies);
}
 
void printHeader(void)
{
    printf("%-6s %-24s %-16s %-6s %-6s\n",
           "ID", "Title", "Author", "Total", "Avail");
}
 
/* Give free copies to the students waiting at the front of the queue */
void processReservations(Book *b)
{
    while (b->availableCopies > 0 && !isEmpty(&b->waitList)) {
        int sid = dequeue(&b->waitList);
        b->availableCopies--;
        addIssue(sid, b->id);
        printf("Copy of Book %d issued automatically to Student %d "
           	"(first in waiting queue).\n", b->id, sid);
    }
}
 
/* ---------- Menu modules ---------- */
void addBook(void)
{
    int id = readInt("Enter Book ID: ");
    Book *b;
    if (searchBookById(id) != NULL) {
        printf("Book ID already exists.\n");
        return;
    }
    b = (Book *)malloc(sizeof(Book));
    b->id = id;
    readText("Enter Title: ", b->title, sizeof(b->title));
    readText("Enter Author: ", b->author, sizeof(b->author));
    b->totalCopies = readInt("Enter Number of Copies: ");
    b->availableCopies = b->totalCopies;
    initQueue(&b->waitList);
    insertBook(b);
    printf("Book added successfully.\n");
}
 
void displayBooks(void)
{
    Book *c = bookHead;
    if (c == NULL) { printf("No books in the library.\n"); return; }
    printHeader();
    while (c != NULL) { printBook(c); c = c->next; }   /* traversal */
}
 
void searchBook(void)
{
    int choice = readInt("Search by 1. Book ID  2. Title: ");
    Book *c;
    if (choice == 1) {
        c = searchBookById(readInt("Enter Book ID: "));
        if (c == NULL) printf("Book not found.\n");
        else { printHeader(); printBook(c); }
    } else {
        char key[30], t1[30], t2[30];
        int found = 0;
        readText("Enter Title: ", key, sizeof(key));
        toLowerStr(key, t1);
        for (c = bookHead; c != NULL; c = c->next) {
        	toLowerStr(c->title, t2);
        	if (strstr(t2, t1) != NULL) {
            	if (!found) printHeader();
            	printBook(c);
            	found = 1;
        	}
        }
        if (!found) printf("Book not found.\n");
    }
}
 
void addStudent(void)
{
    int id = readInt("Enter Student ID: ");
    Student *s;
    if (searchStudent(id) != NULL) {
        printf("Student ID already exists.\n");
        return;
    }
    s = (Student *)malloc(sizeof(Student));
    s->id = id;
    readText("Enter Student Name: ", s->name, sizeof(s->name));
    insertStudent(s);
    printf("Student registered successfully.\n");
}
 
void issueBook(void)
{
    int sid = readInt("Enter Student ID: ");
    int bid = readInt("Enter Book ID: ");
    Book *b = searchBookById(bid);
    if (searchStudent(sid) == NULL) { printf("Student not registered.\n"); return; }
    if (b == NULL) { printf("Book not found.\n"); return; }
    if (searchIssue(sid, bid) != NULL) {
        printf("This student already has this book.\n");
        return;
    }
    if (b->availableCopies == 0) {
        printf("No copy available. Please reserve the book.\n");
        return;
    }
    b->availableCopies--;
    addIssue(sid, bid);
    printf("Book issued successfully. Available copies: %d\n", b->availableCopies);
}
 
void returnBook(void)
{
    int sid = readInt("Enter Student ID: ");
    int bid = readInt("Enter Book ID: ");
    Issue *rec = searchIssue(sid, bid);
    Book *b = searchBookById(bid);
    if (rec == NULL) { printf("No such issue record found.\n"); return; }
    removeIssue(rec);                        /* linked list deletion */
    b->availableCopies++;
    printf("Book returned successfully.\n");
    processReservations(b);                  /* dequeue waiting student */
}
 
void reserveBook(void)
{
    int sid = readInt("Enter Student ID: ");
    int bid = readInt("Enter Book ID: ");
    Book *b = searchBookById(bid);
    Reservation *c;
    int pos = 0;
    if (searchStudent(sid) == NULL) { printf("Student not registered.\n"); return; }
    if (b == NULL) { printf("Book not found.\n"); return; }
    if (b->availableCopies > 0) {
        printf("A copy is available. Issue the book instead.\n");
        return;
    }
    if (searchIssue(sid, bid) != NULL || inQueue(&b->waitList, sid)) {
        printf("Student already holds or is waiting for this book.\n");
        return;
    }
    enqueue(&b->waitList, sid);
    for (c = b->waitList.front; c != NULL; c = c->next) pos++;
    printf("Student added to the waiting queue at position %d.\n", pos);
}
 
void viewWaitingList(void)
{
    int bid = readInt("Enter Book ID: ");
    Book *b = searchBookById(bid);
    Reservation *c;
    int pos = 1;
    if (b == NULL) { printf("Book not found.\n"); return; }
    if (isEmpty(&b->waitList)) { printf("Waiting list is empty.\n"); return; }
    for (c = b->waitList.front; c != NULL; c = c->next)
        printf("%d. Student %d\n", pos++, c->studentId);
}
 
void viewIssued(void)
{
    Issue *c = issueHead;
    if (c == NULL) { printf("No books are issued.\n"); return; }
    printf("%-10s %-8s\n", "Student", "Book");
    for (; c != NULL; c = c->next)
        printf("%-10d %-8d\n", c->studentId, c->bookId);
}
 
/* Bubble sort on the linked list: swap the data of adjacent nodes */
void sortBooks(void)
{
    Book *c;
    int swapped;
    if (bookHead == NULL || bookHead->next == NULL) return;
    do {
        swapped = 0;
        for (c = bookHead; c->next != NULL; c = c->next) {
        	if (c->id > c->next->id) {
            	Book t = *c;
            	c->id = c->next->id;
            	strcpy(c->title, c->next->title);
            	strcpy(c->author, c->next->author);
            	c->totalCopies = c->next->totalCopies;
            	c->availableCopies = c->next->availableCopies;
            	c->waitList = c->next->waitList;
            	c->next->id = t.id;
            	strcpy(c->next->title, t.title);
            	strcpy(c->next->author, t.author);
            	c->next->totalCopies = t.totalCopies;
            	c->next->availableCopies = t.availableCopies;
            	c->next->waitList = t.waitList;
            	swapped = 1;
        	}
        }
    } while (swapped);
    printf("Books sorted by Book ID.\n");
}
 
void freeAll(void)
{
    while (bookHead != NULL) {
        Book *b = bookHead;
        bookHead = b->next;
        while (!isEmpty(&b->waitList)) dequeue(&b->waitList);
        free(b);
    }
    while (studentHead != NULL) {
        Student *s = studentHead;
        studentHead = s->next;
        free(s);
    }
    while (issueHead != NULL) removeIssue(issueHead);
}
 
int main(void)
{
    int choice;
    do {
        printf("\n===== SMART LIBRARY MANAGEMENT SYSTEM =====\n");
        printf("1. Add Book\n2. Display Books\n3. Search Book\n");
        printf("4. Register Student\n5. Issue Book\n6. Return Book\n");
        printf("7. Reserve Book\n8. View Waiting List\n9. View Issued Books\n");
        printf("10. Sort Books by ID\n0. Exit\n");
        choice = readIntMin("Enter your choice: ", 0);
        switch (choice) {
        	case 1: addBook(); break;
        	case 2: displayBooks(); break;
        	case 3: searchBook(); break;
        	case 4: addStudent(); break;
        	case 5: issueBook(); break;
        	case 6: returnBook(); break;
        	case 7: reserveBook(); break;
        	case 8: viewWaitingList(); break;
        	case 9: viewIssued(); break;
        	case 10: sortBooks(); break;
        	case 0: printf("Thank you.\n"); break;
        	default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
    freeAll();
    return 0;
}








