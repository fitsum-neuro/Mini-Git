MiniGit: A Custom Version Control System
This project is an educational, lightweight version of Git built from scratch in C++. It is designed to help understand the core principles behind modern version control systems, including the use of hashing, trees, and directed acyclic graphs (DAGs) to track file changes.

This system simulates a local-only Git experience through a command-line interface (CLI).

Features Implemented (So far)
init: Initializes a new, empty MiniGit repository in the current directory.
add <filename>: Stages file contents for the next commit.
commit -m "<message>": Records a snapshot of the staged files permanently into the repository's history.

Prerequisites
Before you can build and run MiniGit, you must have the following tools installed on your system:
>>A C++ Compiler: A compiler that supports the C++17 standard is required (e.g., g++ version 8 or higher).
>>GNU Make: The make utility is used to build the project from the source code.

To compile the project
>> git clone https://github.com/your-username/your-repository-name.git

Navigate to the Project Directory

Compile the Code
>>Run the make command. This will read the Makefile and build the executable file.

How to Use MiniGit

 >>create a separate project folder to test it in.

 >> Run the init command. 
  ..\MiniGit\minigit.exe init (depends on your path)
  How to Create and Add a file
  >>echo "Hello, version control!" > readme.md (creates a file)
  >> ..\MiniGit\minigit.exe add readme.md (stages the file)
  
  Commiting change
  >> ..\Mini-Git\minigit.exe commit -m "LOL"
  