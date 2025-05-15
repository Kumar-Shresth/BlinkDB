# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# Executables
SERVER = blinkdb_server
CLIENT = blinkdb_client
REPL = repl
STORAGE = storageEngine.o

# Source Files
SERVER_SRC = server.cpp storageEngine.cpp
CLIENT_SRC = client.cpp
REPL_SRC = repl.cpp storageEngine.cpp
STORAGE_SRC = storageEngine.cpp

# Dependencies
SERVER_DEPS = storageEngine.h
CLIENT_DEPS = 
REPL_DEPS = storageEngine.h
STORAGE_DEPS = storageEngine.h

# Doxygen Documentation
DOXYGEN = doxygen
DOXYFILE = Doxyfile

# Build Server (Part B)
$(SERVER): $(SERVER_SRC) $(SERVER_DEPS)
	$(CXX) $(CXXFLAGS) -o $(SERVER) $(SERVER_SRC)

# Build Client (Part B)
$(CLIENT): $(CLIENT_SRC) $(CLIENT_DEPS)
	$(CXX) $(CXXFLAGS) -o $(CLIENT) $(CLIENT_SRC)

# Build REPL (Part A)
$(REPL): $(REPL_SRC) $(REPL_DEPS)
	$(CXX) $(CXXFLAGS) -o $(REPL) $(REPL_SRC)

# Build Storage Engine (Part A)
$(STORAGE): $(STORAGE_SRC) $(STORAGE_DEPS)
	$(CXX) $(CXXFLAGS) -c $(STORAGE_SRC)

# Generate Documentation (HTML + PDF)
doc:
	$(DOXYGEN) $(DOXYFILE)
	cd latex && make

# Clean all generated files
clean:
	rm -f $(SERVER) $(CLIENT) $(REPL) $(STORAGE) *.o
	rm -rf html latex

.PHONY: clean doc
