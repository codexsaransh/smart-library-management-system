# Smart Library Management System Using Data Structures & Algorithms in C

A modular, console-based library automation system written in standard C (C99), demonstrating practical implementations of fundamental Data Structures and Algorithms (DSA) including Singly Linked Lists, First-In-First-Out (FIFO) Queues, Linear Search, and Bubble Sort, alongside atomic text-file persistence.

Developed for **B.Tech Information Technology (Semester III)** coursework under **Guru Gobind Singh Indraprastha University (GGSIPU)**.

---

## Key Highlights & Core DSA Implementations

1. **Singly Linked Lists (`malloc()` / `free()`)**:
   - Dynamic in-memory allocation for Books, Registered Students, and Active Borrowing Records.
   - Eliminates static array size boundaries; scales dynamically with zero memory waste.
   - Clean deletion and list traversal routines with verified heap deallocation.

2. **FIFO Queue-Based Fair Reservation Engine**:
   - Each `Book` node encapsulates its own independent `Queue` structure equipped with `front`, `rear`, and `count` pointers.
   - Guarantees $O(1)$ Enqueue and Dequeue operations.
   - **Automated Waitlist Dispatch**: When an issued book is returned or inventory is augmented, the system automatically dispatches the available copy to the student at the front of that specific book's queue.

3. **Linear Search**:
   - Supports search by unique Book ID ($O(1)$ best, $O(n)$ worst) and case-insensitive partial substring title search.
   - Internal linear lookup ensures duplicate prevention for Book IDs, Student IDs, and multiple copies loaned to the same student.

4. **In-Place Bubble Sort on Linked Nodes**:
   - Orders the book collection by Book ID.
   - Swaps node data fields in-place while keeping pointer connections intact. Includes early-termination flag (`swapped`) achieving $O(n)$ best-case runtime.

5. **Crash-Resilient File Persistence**:
   - Employs atomic write operations via temporary swap files (`books.tmp -> books.txt`), ensuring file integrity even in unexpected aborts.
   - Formats data with pipe-separated delimiters (`|`), preserving titles and names containing spaces.

---

## Project Structure

```text
smart-library-management-system/
├── library.c              # Integrated C implementation (~1,300 lines)
├── Makefile               # Cross-platform build script
├── build.bat              # Windows compilation & run script
├── books.txt              # Persistent book records
├── students.txt           # Persistent registered student records
├── issued_books.txt       # Active loan records
├── reservations.txt       # Active waitlist reservation queues
├── .gitignore             # Ignored compilation binaries & swap files
└── README.md              # Project documentation
```

---

## Build & Execution Instructions

### Prerequisites
- GCC / Clang / MinGW C compiler (supporting C99 or later).
- Standard C libraries: `<stdio.h>`, `<stdlib.h>`, `<string.h>`.

### Compilation via GCC
```bash
# Standard compilation
gcc library.c -o library

# Strict academic / warning-free flags
gcc -Wall -Wextra -std=c99 -pedantic -O2 library.c -o library
```

### Running the System
- **Linux / macOS**:
  ```bash
  ./library
  ```
- **Windows (Command Prompt / PowerShell)**:
  ```powershell
  .\library.exe
  ```
- **Using Makefile**:
  ```bash
  make run
  ```

---

## Complexity Matrix

| Operation | DSA Concept | Time Complexity (Best / Avg / Worst) | Space Complexity |
| :--- | :--- | :--- | :--- |
| **Add Book / Student** | Linked List Insertion | $O(1) \text{ / } O(n) \text{ / } O(n)$ | $O(1)$ |
| **Search by Book ID** | Linear Search | $O(1) \text{ / } O(n) \text{ / } O(n)$ | $O(1)$ |
| **Search by Title** | Substring Linear Scan | $O(n) \text{ / } O(n) \text{ / } O(n)$ | $O(1)$ |
| **Sort Books by ID** | In-Place Bubble Sort | $O(n) \text{ / } O(n^2) \text{ / } O(n^2)$ | $O(1)$ |
| **Enqueue Reservation** | FIFO Queue Insertion | $O(1) \text{ / } O(1) \text{ / } O(1)$ | $O(1)$ |
| **Dequeue & Auto-Issue** | FIFO Queue Dequeue | $O(1) \text{ / } O(1) \text{ / } O(1)$ | $O(1)$ |
| **Persistence Sync** | File Stream I/O | $O(n) \text{ / } O(n) \text{ / } O(n)$ | $O(1)$ |

---

## Menu Options

1. **Add New Book**: Dynamic addition with validation.
2. **Display All Books**: Full tabular inventory view.
3. **Search Book**: Fast lookup by ID or title substring.
4. **Issue Book**: Availability check, loan validation, copy deduction.
5. **Return Book**: Record unlinking, copy increment, automated FIFO waitlist fulfillment.
6. **Reserve Book**: Queues waiting student if zero copies remain.
7. **Add Student**: Member onboarding with duplicate ID filtering.
8. **Display Student Records**: Lists all active student registrations.
9. **Sort Books by ID**: Bubble sort ordering with pass-by-pass telemetry.
10. **View Issued Books**: Real-time tracking of loaned titles and borrowers.
11. **View Reservation Waiting List**: Per-book queue inspection from front to rear.
12. **Exit**: Flushes all buffers to disk, cleanly frees allocated heap nodes, and exits.
13. **Update Book Details**: Inventory and metadata management.
14. **Delete Book**: Guarded deletion (blocked if copies are currently checked out).

---

## License & Academic Notice
This project is developed for educational and academic evaluation purposes. Feel free to reference and adapt with appropriate academic attribution.
