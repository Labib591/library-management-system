# Library Management System

A comprehensive C++ library management system that provides efficient book tracking, borrower management, and advanced category-based organization features.

## Features

### Book Management
- Add and remove books with detailed information (title, author, ISBN, category)
- Track book availability status
- Sort books by title or author
- Persistent storage using CSV files

### Borrower Management
- Maintain borrower records with unique IDs
- Track borrowed books for each user
- Handle book checkouts and returns
- Store borrower history

### Category Organization
- Hierarchical category structure
- Category relationship analysis
- Book recommendations based on category relationships
- Visual representation of category connections

### Advanced Features
- Transaction history tracking
- Category-based book recommendations
- Relationship strength analysis between book categories
- Interactive search and display capabilities

## Data Structures Used
- Vectors for book and borrower collections
- Queue for reservation management
- Stack for transaction history
- Maps and Sets for category organization
- Tree structure for hierarchical categories
- Graph structure for category relationships

## File Structure
LibrarySystem/
├── library_system.cpp    # Main source code
├── books.csv            # Book database
├── borrowers.csv        # Borrower records
└── README.md           # Documentation

## Classes

### Book
- Manages individual book information
- Handles availability status
- Provides CSV format conversion

### Borrower
- Maintains borrower details
- Tracks borrowed books
- Manages book checkouts and returns

### LibraryManager
- Core system management
- Handles all operations between books and borrowers
- Manages file operations and data persistence

### CategoryNode & CategoryGraph
- Implements category organization
- Manages relationships between categories
- Provides recommendation system

## Getting Started

### Prerequisites
- C++ compiler (C++11 or later)
- Standard Template Library (STL)

### Compilation
g++ -std=c++11 library_system.cpp -o library_system

### Running the Program
./library_system

## Usage Examples

### Adding a Book
Book newBook("The Great Gatsby", "F. Scott Fitzgerald", "978-0743273565", "Fiction");
libraryManager.addBook(newBook);

### Borrowing a Book
libraryManager.borrowBook("978-0743273565", "BORROWER001");

### Searching by Category
libraryManager.searchByCategory();  // Interactive category search

### Displaying Books
libraryManager.displayBooks();  // Show all books
libraryManager.displayBooksByCategory();  // Show books organized by category

## Data Storage

### books.csv Format
Title,Author,ISBN,Available,Category
The Great Gatsby,F. Scott Fitzgerald,978-0743273565,1,Fiction

### borrowers.csv Format
ID,Name,BorrowedBooks
BORROWER001,John Smith,978-0743273565;978-0451524935

## Features in Detail

### Category Analysis
- Tracks relationships between book categories
- Provides strength metrics for category connections
- Generates book recommendations based on category relationships

### Transaction Tracking
- Records all book checkouts and returns
- Maintains history of user interactions
- Provides audit trail for library operations

## Contributing
Feel free to submit issues and enhancement requests.

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments
- Inspired by modern library management systems
- Utilizes efficient data structures and algorithms
- Implements best practices in C++ programming
