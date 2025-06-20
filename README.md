MiniGit: A Custom Version Control System
This project is an educational, lightweight version of Git built from scratch in C++. It is designed to help understand the core principles behind modern version control systems, including the use of hashing, trees, and directed acyclic graphs (DAGs) to track file changes.

This system simulates a local-only Git experience through a command-line interface (CLI).

Features Implemented
- `init`: Initializes a new, empty MiniGit repository in the current directory.
- `add <filename>`: Stages file contents for the next commit.
- `commit -m "<message>"`: Records a snapshot of the staged files permanently into the repository's history.
- `log`: Views the commit history, showing commit hashes and messages from HEAD backward.
- `branch <branch-name>`: Creates a new branch pointer to the current commit.
- `switch <branch-name>`: Switches HEAD to the specified branch.
- `diff`: Implement diff <commit1> <commit2> to show line-by-line differences.

Prerequisites
Before you can build and run MiniGit, you must have the following tools installed on your system:
>>A C++ Compiler: A compiler that supports the C++17 standard is required (e.g., g++ version 8 or higher).
>>GNU Make: The make utility is used to build the project from the source code.

To compile the project
>> git clone https://github.com/fitsum-neuro/Mini-Git.git

Navigate to the Project Directory

Compile the Code
>>Run the make command. This will read the Makefile and build the executable file.

For window (..\your-path\minigit.exe <feature eg : init>)
How to Use MiniGit (Linux)

1. **Initialize a repository:**
   ```bash
   ./minigit init
   ```
2. **Create and add a file:**
   ```bash
   echo "Hello, version control!" > readme.md
   ./minigit add readme.md
   ```
3. **Commit changes:**
   ```bash
   ./minigit commit -m "Initial commit"
   ```
4. **View commit history:**
   ```bash
   ./minigit log
   ```
5. **Create a branch:**
   ```bash
   ./minigit branch feature1
   ```
6. **Switch branches:**
   ```bash
   ./minigit switch feature1
   ```
7. **Diff Viewer:**
  ```bash
  ./minigit diff <commit1> <commit2>
  ```

---

### Branch Management
- Branches are stored in `.minigit/refs/<branch-name>` and point to specific commits.
- The current branch is tracked in `.minigit/CURRENT_BRANCH`.
- When you switch branches, HEAD is updated to the commit pointed to by the branch.

### Viewing Commit History
- The `log` command traverses from the current HEAD backward, showing each commit's hash, date, and message.

---

**Note:**
- All commands are run from the project root directory.
- This project is for educational use and does not implement all safety checks or features of full Git.