#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <limits>

// Book class definition
class Book {
private:
    std::string title;
    std::string author;
    std::string isbn;
    bool isAvailable;
    std::string category;

public:
    Book(const std::string& t = "", const std::string& a = "", const std::string& i = "", 
         const std::string& cat = "") 
        : title(t), author(a), isbn(i), isAvailable(true), category(cat) {}

    // Getters
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    std::string getISBN() const { return isbn; }
    bool getAvailability() const { return isAvailable; }
    std::string getCategory() const { return category; }

    // Setters
    void setTitle(const std::string& t) { title = t; }
    void setAuthor(const std::string& a) { author = a; }
    void setISBN(const std::string& i) { isbn = i; }
    void setAvailability(bool status) { isAvailable = status; }
    void setCategory(const std::string& cat) { category = cat; }

    // CSV format conversion
    std::string toCSV() const {
        return title + "," + author + "," + isbn + "," + 
               std::to_string(isAvailable) + "," + category;
    }
};

// Tree structure for category-based organization
struct CategoryNode {
    std::string category;
    std::vector<Book> books;
    std::vector<CategoryNode*> subcategories;
    
    CategoryNode(const std::string& cat) : category(cat) {}
    
    ~CategoryNode() {
        for (auto subcat : subcategories) {
            delete subcat;
        }
    }
    
    void addBook(const Book& book) {
        books.push_back(book);
    }
    
    void display(int level = 0) const {
        std::string indent(level * 4, ' ');
        std::cout << indent << "Category: " << category << "\n";
        std::cout << indent << "Books:\n";
        for (const auto& book : books) {
            std::cout << indent << "    - " << book.getTitle() 
                     << " by " << book.getAuthor() 
                     << " (ISBN: " << book.getISBN() << ")"
                     << " [" << (book.getAvailability() ? "Available" : "Borrowed") << "]\n";
        }
        for (const auto& subcat : subcategories) {
            subcat->display(level + 1);
        }
    }
};

// Graph structure for category relationships
struct CategoryGraph {
    struct Edge {
        std::string from;
        std::string to;
        int weight;  // Number of books that share these categories
        
        Edge(const std::string& f, const std::string& t, int w) 
            : from(f), to(t), weight(w) {}
    };
    
    std::map<std::string, std::vector<Edge>> adjacencyList;
    
    void addEdge(const std::string& from, const std::string& to, int weight) {
        adjacencyList[from].push_back(Edge(from, to, weight));
        adjacencyList[to].push_back(Edge(to, from, weight));  // Undirected graph
    }
    
    void display() const {
        std::cout << "\nCategory Relationships Graph:\n";
        std::cout << "============================\n";
        
        // Display each category and its connections
        for (const auto& [category, edges] : adjacencyList) {
            if (!edges.empty()) {
                std::cout << category << " is connected to:\n";
                for (const auto& edge : edges) {
                    std::cout << "  - " << edge.to 
                             << " (Strength: " << edge.weight << " shared books)\n";
                }
                std::cout << "\n";
            }
        }
    }
};

// Borrower class definition
class Borrower {
private:
    std::string id;
    std::string name;
    std::vector<std::string> borrowedBooks; // Stores ISBNs of borrowed books

public:
    Borrower(const std::string& i = "", const std::string& n = "")
        : id(i), name(n) {}

    // Getters
    std::string getID() const { return id; }
    std::string getName() const { return name; }
    const std::vector<std::string>& getBorrowedBooks() const { return borrowedBooks; }

    // Setters
    void setID(const std::string& i) { id = i; }
    void setName(const std::string& n) { name = n; }

    // Borrowing operations
    void borrowBook(const std::string& isbn) {
        borrowedBooks.push_back(isbn);
    }

    void returnBook(const std::string& isbn) {
        auto it = std::find(borrowedBooks.begin(), borrowedBooks.end(), isbn);
        if (it != borrowedBooks.end()) {
            borrowedBooks.erase(it);
        }
    }

    // CSV format conversion
    std::string toCSV() const {
        std::string books;
        for (const auto& isbn : borrowedBooks) {
            books += isbn + ";";
        }
        return id + "," + name + "," + books;
    }
};

// Library Manager class definition
class LibraryManager {
private:
    std::vector<Book> books;
    std::vector<Borrower> borrowers;
    std::queue<std::pair<std::string, std::string>> reservations; // pair of ISBN and borrower ID
    std::stack<std::string> recentTransactions;
    std::map<std::string, std::vector<std::string>> categoryGraph; // for book recommendations

    // File paths
    const std::string BOOKS_FILE = "books.csv";
    const std::string BORROWERS_FILE = "borrowers.csv";

public:
    LibraryManager() {
        loadData();
    }

    // Data persistence methods
    void loadData() {
        loadBooks();
        loadBorrowers();
    }

    void saveData() {
        saveBooks();
        saveBorrowers();
    }

    // Book management
    void addBook(const Book& book) {
        books.push_back(book);
        updateCategoryGraph(book);
    }

    void removeBook(const std::string& isbn) {
        books.erase(
            std::remove_if(books.begin(), books.end(),
                [&isbn](const Book& b) { return b.getISBN() == isbn; }),
            books.end());
    }

    // Borrower management
    void addBorrower(const Borrower& borrower) {
        borrowers.push_back(borrower);
    }

    // Borrowing operations
    bool borrowBook(const std::string& isbn, const std::string& borrowerId) {
        auto bookIt = std::find_if(books.begin(), books.end(),
            [&isbn](const Book& b) { return b.getISBN() == isbn; });
        
        if (bookIt != books.end() && bookIt->getAvailability()) {
            auto borrowerIt = std::find_if(borrowers.begin(), borrowers.end(),
                [&borrowerId](const Borrower& b) { return b.getID() == borrowerId; });
            
            if (borrowerIt != borrowers.end()) {
                bookIt->setAvailability(false);
                borrowerIt->borrowBook(isbn);
                recentTransactions.push("Borrow: " + isbn + " by " + borrowerId);
                return true;
            }
        }
        return false;
    }

    bool returnBook(const std::string& isbn, const std::string& borrowerId) {
        auto bookIt = std::find_if(books.begin(), books.end(),
            [&isbn](const Book& b) { return b.getISBN() == isbn; });
        
        if (bookIt != books.end()) {
            auto borrowerIt = std::find_if(borrowers.begin(), borrowers.end(),
                [&borrowerId](const Borrower& b) { return b.getID() == borrowerId; });
            
            if (borrowerIt != borrowers.end()) {
                bookIt->setAvailability(true);
                borrowerIt->returnBook(isbn);
                recentTransactions.push("Return: " + isbn + " by " + borrowerId);
                return true;
            }
        }
        return false;
    }

    // Display methods
    void displayBooks() const {
        std::cout << "\nLibrary Books:\n";
        std::cout << "----------------------------------------\n";
        for (const auto& book : books) {
            std::cout << "Title: " << book.getTitle() << "\n"
                     << "Author: " << book.getAuthor() << "\n"
                     << "ISBN: " << book.getISBN() << "\n"
                     << "Status: " << (book.getAvailability() ? "Available" : "Borrowed") << "\n"
                     << "Category: " << book.getCategory() << "\n"
                     << "----------------------------------------\n";
        }
    }

    void displayBooksByCategory() {
        // Create root node
        CategoryNode* root = new CategoryNode("Root");
        
        // Create category map
        std::map<std::string, CategoryNode*> categoryMap;
        
        // First pass: Create category nodes
        for (const auto& book : books) {
            std::string category = book.getCategory();
            if (categoryMap.find(category) == categoryMap.end()) {
                CategoryNode* node = new CategoryNode(category);
                categoryMap[category] = node;
                root->subcategories.push_back(node);
            }
        }
        
        // Second pass: Add books to categories
        for (const auto& book : books) {
            categoryMap[book.getCategory()]->addBook(book);
        }
        
        // Sort subcategories alphabetically
        std::sort(root->subcategories.begin(), root->subcategories.end(),
            [](const CategoryNode* a, const CategoryNode* b) {
                return a->category < b->category;
            });
        
        // Display the tree
        std::cout << "\nLibrary Books by Category:\n";
        std::cout << "========================\n";
        for (const auto& catNode : root->subcategories) {
            catNode->display();
            std::cout << "------------------------\n";
        }
        
        // Cleanup
        delete root;
    }

    void searchByCategory() {
        std::cout << "\nAvailable Categories:\n";
        std::set<std::string> categories;
        for (const auto& book : books) {
            categories.insert(book.getCategory());
        }
        
        for (const auto& category : categories) {
            std::cout << "- " << category << "\n";
        }
        
        std::string searchCategory;
        std::cout << "\nEnter category to search: ";
        std::getline(std::cin, searchCategory);
        
        std::cout << "\nBooks in category '" << searchCategory << "':\n";
        std::cout << "----------------------------------------\n";
        bool found = false;
        
        for (const auto& book : books) {
            if (book.getCategory() == searchCategory) {
                std::cout << "Title: " << book.getTitle() << "\n"
                         << "Author: " << book.getAuthor() << "\n"
                         << "ISBN: " << book.getISBN() << "\n"
                         << "Status: " << (book.getAvailability() ? "Available" : "Borrowed") << "\n"
                         << "----------------------------------------\n";
                found = true;
            }
        }
        
        if (!found) {
            std::cout << "No books found in category '" << searchCategory << "'\n";
        }
    }

    // Sorting methods
    void sortBooksByTitle() {
        std::sort(books.begin(), books.end(),
            [](const Book& a, const Book& b) { return a.getTitle() < b.getTitle(); });
    }

    void sortBooksByAuthor() {
        std::sort(books.begin(), books.end(),
            [](const Book& a, const Book& b) { return a.getAuthor() < b.getAuthor(); });
    }

    void analyzeCategories() {
        std::cout << "\nAnalyzing Library Categories...\n\n";
        
        // First, display category statistics
        std::map<std::string, int> categoryCount;
        for (const auto& book : books) {
            categoryCount[book.getCategory()]++;
        }
        
        std::cout << "Category Statistics:\n";
        std::cout << "===================\n";
        for (const auto& [category, count] : categoryCount) {
            std::cout << category << ": " << count << " books\n";
        }
        std::cout << "\n";

        // Create and populate the category graph
        CategoryGraph graph;
        
        // Define category relationships and their strengths
        const std::vector<std::tuple<std::string, std::string, int>> relationships = {
            {"Fiction", "Fantasy", 3},
            {"Fiction", "Mystery", 2},
            {"Fiction", "Romance", 2},
            {"Fantasy", "Science Fiction", 3},
            {"Science Fiction", "Fiction", 2},
            {"Technical", "Science Fiction", 1}
        };

        // Add edges for defined relationships
        for (const auto& [cat1, cat2, strength] : relationships) {
            // Only add edge if both categories exist in our library
            if (categoryCount.find(cat1) != categoryCount.end() && 
                categoryCount.find(cat2) != categoryCount.end()) {
                graph.addEdge(cat1, cat2, strength);
            }
        }

        // Display category relationships
        std::cout << "Category Relationships:\n";
        std::cout << "=====================\n";
        for (const auto& [category, edges] : graph.adjacencyList) {
            if (!edges.empty()) {
                std::cout << category << " is connected to:\n";
                for (const auto& edge : edges) {
                    std::cout << "  - " << edge.to;
                    switch (edge.weight) {
                        case 3:
                            std::cout << " (Strong relationship)\n";
                            break;
                        case 2:
                            std::cout << " (Moderate relationship)\n";
                            break;
                        case 1:
                            std::cout << " (Weak relationship)\n";
                            break;
                    }
                }
                std::cout << "\n";
            }
        }

        // Display book recommendations based on relationships
        std::cout << "Sample Cross-Category Recommendations:\n";
        std::cout << "===================================\n";
        for (const auto& [category, edges] : graph.adjacencyList) {
            if (!edges.empty()) {
                std::cout << "If you like " << category << ", you might also enjoy:\n";
                std::set<std::string> recommended;
                
                // Get books from related categories
                for (const auto& edge : edges) {
                    for (const auto& book : books) {
                        if (book.getCategory() == edge.to && book.getAvailability()) {
                            recommended.insert(book.getTitle() + " (" + edge.to + ")");
                            if (recommended.size() >= 2) break;  // Limit to 2 recommendations per category
                        }
                    }
                }
                
                for (const auto& title : recommended) {
                    std::cout << "  - " << title << "\n";
                }
                std::cout << "\n";
            }
        }
    }
    
    void recommendBooks(const std::string& category, std::set<std::string>& visited, 
                       std::vector<std::string>& recommendations, int depth = 0) {
        if (depth >= 3 || visited.count(category) > 0) return;  // Limit recursion depth
        
        visited.insert(category);
        
        // Add books from current category
        for (const auto& book : books) {
            if (book.getCategory() == category && book.getAvailability()) {
                recommendations.push_back(book.getTitle());
            }
        }
        
        // Recursively explore related categories
        std::vector<std::string> relatedCategories;
        if (category == "Fiction") {
            relatedCategories = {"Fantasy", "Mystery", "Romance"};
        } else if (category == "Fantasy") {
            relatedCategories = {"Fiction", "Science Fiction"};
        } else if (category == "Science Fiction") {
            relatedCategories = {"Fantasy", "Fiction"};
        } else if (category == "Technical") {
            relatedCategories = {"Science Fiction"};
        }
        
        for (const auto& relatedCategory : relatedCategories) {
            recommendBooks(relatedCategory, visited, recommendations, depth + 1);
        }
    }
    
    void getBookRecommendations() {
        std::cout << "\nAvailable Categories:\n";
        std::set<std::string> categories;
        for (const auto& book : books) {
            categories.insert(book.getCategory());
        }
        
        for (const auto& category : categories) {
            std::cout << "- " << category << "\n";
        }
        
        std::string startCategory;
        std::cout << "\nEnter starting category for recommendations: ";
        std::getline(std::cin, startCategory);
        
        // Check if category exists
        if (categories.find(startCategory) == categories.end()) {
            std::cout << "Category not found!\n";
            return;
        }
        
        std::set<std::string> visited;
        std::vector<std::string> recommendations;
        recommendBooks(startCategory, visited, recommendations);
        
        std::cout << "\nRecommended Books (based on category '" << startCategory << "'):\n";
        std::cout << "=================================================\n";
        if (recommendations.empty()) {
            std::cout << "No recommendations found.\n";
        } else {
            for (size_t i = 0; i < recommendations.size(); ++i) {
                std::cout << (i + 1) << ". " << recommendations[i] << "\n";
            }
        }
    }

private:
    void loadBooks() {
        std::ifstream file(BOOKS_FILE);
        if (!file.is_open()) {
            std::cout << "Warning: Could not open " << BOOKS_FILE << ". Starting with empty book list.\n";
            return;
        }

        std::string line;
        // Skip header line
        std::getline(file, line);
        
        while (std::getline(file, line)) {
            size_t pos = 0;
            std::vector<std::string> tokens;
            std::string token;
            
            // Split the line by commas, handling potential commas in the title/author
            while ((pos = line.find(",")) != std::string::npos) {
                token = line.substr(0, pos);
                tokens.push_back(token);
                line.erase(0, pos + 1);
            }
            tokens.push_back(line);  // Add the last token

            if (tokens.size() >= 5) {
                Book book(tokens[0], tokens[1], tokens[2], tokens[4]);
                book.setAvailability(tokens[3] == "1");
                books.push_back(book);
            }
        }
        file.close();
        std::cout << "Loaded " << books.size() << " books from " << BOOKS_FILE << "\n";
    }

    void loadBorrowers() {
        std::ifstream file(BORROWERS_FILE);
        if (!file.is_open()) {
            std::cout << "Warning: Could not open " << BORROWERS_FILE << ". Starting with empty borrower list.\n";
            return;
        }

        std::string line;
        // Skip header line
        std::getline(file, line);
        
        while (std::getline(file, line)) {
            size_t pos = 0;
            std::vector<std::string> tokens;
            
            // Split by comma
            while ((pos = line.find(",")) != std::string::npos) {
                tokens.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            tokens.push_back(line);

            if (tokens.size() >= 2) {
                Borrower borrower(tokens[0], tokens[1]);
                // If there are borrowed books (tokens[2]), parse them
                if (tokens.size() > 2 && !tokens[2].empty()) {
                    std::string books = tokens[2];
                    size_t pos = 0;
                    while ((pos = books.find(";")) != std::string::npos) {
                        std::string isbn = books.substr(0, pos);
                        if (!isbn.empty()) {
                            borrower.borrowBook(isbn);
                        }
                        books.erase(0, pos + 1);
                    }
                    if (!books.empty()) {
                        borrower.borrowBook(books);
                    }
                }
                borrowers.push_back(borrower);
            }
        }
        file.close();
        std::cout << "Loaded " << borrowers.size() << " borrowers from " << BORROWERS_FILE << "\n";
    }

    void saveBooks() {
        std::ofstream file(BOOKS_FILE);
        if (!file.is_open()) {
            std::cout << "Error: Could not save books to " << BOOKS_FILE << "\n";
            return;
        }

        // Write header
        file << "Title,Author,ISBN,Available,Category\n";
        
        // Write book data
        for (const auto& book : books) {
            file << book.toCSV() << "\n";
        }
        file.close();
        std::cout << "Saved " << books.size() << " books to " << BOOKS_FILE << "\n";
    }

    void saveBorrowers() {
        std::ofstream file(BORROWERS_FILE);
        if (!file.is_open()) {
            std::cout << "Error: Could not save borrowers to " << BORROWERS_FILE << "\n";
            return;
        }

        // Write header
        file << "ID,Name,BorrowedBooks\n";
        
        // Write borrower data
        for (const auto& borrower : borrowers) {
            file << borrower.toCSV() << "\n";
        }
        file.close();
        std::cout << "Saved " << borrowers.size() << " borrowers to " << BORROWERS_FILE << "\n";
    }

    void updateCategoryGraph(const Book& book) {
        categoryGraph[book.getCategory()].push_back(book.getISBN());
    }
};

// Helper functions for user input
void displayMenu() {
    std::cout << "\nLibrary Management System\n";
    std::cout << "1. Add Book\n";
    std::cout << "2. Add Borrower\n";
    std::cout << "3. Borrow Book\n";
    std::cout << "4. Return Book\n";
    std::cout << "5. Display Books\n";
    std::cout << "6. Sort Books by Title\n";
    std::cout << "7. Sort Books by Author\n";
    std::cout << "8. Display Books by Category\n";
    std::cout << "9. Search Books by Category\n";
    std::cout << "10. Show Category Analytics\n";
    std::cout << "11. Get Book Recommendations\n";
    std::cout << "12. Exit\n";
    std::cout << "Enter your choice: ";
}

Book inputBookDetails() {
    std::string title, author, isbn, category;
    
    std::cout << "Enter book title: ";
    std::getline(std::cin, title);
    
    std::cout << "Enter author name: ";
    std::getline(std::cin, author);
    
    std::cout << "Enter ISBN: ";
    std::getline(std::cin, isbn);
    
    std::cout << "Enter category: ";
    std::getline(std::cin, category);
    
    return Book(title, author, isbn, category);
}

Borrower inputBorrowerDetails() {
    std::string id, name;
    
    std::cout << "Enter borrower ID: ";
    std::getline(std::cin, id);
    
    std::cout << "Enter borrower name: ";
    std::getline(std::cin, name);
    
    return Borrower(id, name);
}

// Main function
int main() {
    LibraryManager library;
    int choice;
    
    while (true) {
        displayMenu();
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: {
                Book book = inputBookDetails();
                library.addBook(book);
                std::cout << "Book added successfully!\n";
                break;
            }
            case 2: {
                Borrower borrower = inputBorrowerDetails();
                library.addBorrower(borrower);
                std::cout << "Borrower added successfully!\n";
                break;
            }
            case 3: {
                std::string isbn, borrowerId;
                std::cout << "Enter ISBN: ";
                std::getline(std::cin, isbn);
                std::cout << "Enter borrower ID: ";
                std::getline(std::cin, borrowerId);
                
                if (library.borrowBook(isbn, borrowerId)) {
                    std::cout << "Book borrowed successfully!\n";
                } else {
                    std::cout << "Failed to borrow book. Please check availability and borrower ID.\n";
                }
                break;
            }
            case 4: {
                std::string isbn, borrowerId;
                std::cout << "Enter ISBN: ";
                std::getline(std::cin, isbn);
                std::cout << "Enter borrower ID: ";
                std::getline(std::cin, borrowerId);
                
                if (library.returnBook(isbn, borrowerId)) {
                    std::cout << "Book returned successfully!\n";
                } else {
                    std::cout << "Failed to return book. Please check ISBN and borrower ID.\n";
                }
                break;
            }
            case 5:
                library.displayBooks();
                break;
            case 6:
                library.sortBooksByTitle();
                std::cout << "Books sorted by title!\n";
                library.displayBooks();
                break;
            case 7:
                library.sortBooksByAuthor();
                std::cout << "Books sorted by author!\n";
                library.displayBooks();
                break;
            case 8:
                library.displayBooksByCategory();
                break;
            case 9:
                library.searchByCategory();
                break;
            case 10:
                library.analyzeCategories();
                break;
            case 11:
                library.getBookRecommendations();
                break;
            case 12:
                library.saveData();
                std::cout << "Thank you for using the Library Management System!\n";
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
    
    return 0;
}
