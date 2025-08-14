#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <regex>

#define FOLDER_DEFAULT "Files/"
// only FOLDER_STATIC is included in the repo
#define FOLDER_STATIC "FileStatic/"
// DO NOT CHANGE  THE LAST VALUE
//   it's an empty string to signal the end of vector
#define IMPLEMENTED_EXTENSIONS {".txt", ".md", ""}  
#define END_WITH_NEW_LINE false
#define PATTERN "\\s*#.*"
#define ESCAPE_REGEX(T) regex_replace(T, regex(R"([.^$|()\\*+?{}[\]])"), R"(\$&)")
#define PATTERN_A(T) (R"(\s*#+\s*)" + ESCAPE_REGEX(T) + R"(\s*(?:#.*|$))")
#define PATTERN_B(T) (R"(\s*#+\s*([^#]*)" + ESCAPE_REGEX(T) + "[^#]*).*")
#define PATTERN_C R"(\s*#+\s*([^#]+))"
#define START_HIGHLIGHT "\033[31m"  // Red text
#define END_HIGHLIGHT "\033[0m"
#define HIGHLIGHT(T) (START_HIGHLIGHT + T + END_HIGHLIGHT)

namespace fs = std::filesystem;
using namespace std;

void print_matches(streampos pos, ifstream& file, regex pattern);

string implemented_extensions[] = IMPLEMENTED_EXTENSIONS;
bool is_extension(string ext) {
    for (int i=0; !implemented_extensions[i].empty(); i++) {
        if (ext == implemented_extensions[i])
            return true;
    }
    return false;
}

class Parrot {
    private:
    vector <streampos> matchA_positions;
    vector <streampos> matchB_positions;
    vector <string> matchB_targets;
    string filepath;
    ifstream file;
    public:
    Parrot(string file_path):
    filepath(file_path), file() {
        // Default constructor
    }
    Parrot(string file_path, string target, bool case_flag=false):
    filepath(file_path), file() {
        search(target, case_flag);
    }
    void search(string target, bool case_flag=false);
    ~Parrot() {
        file.close();
    }
    void close() {
        file.close();
    }
    operator bool() const {
        return file.is_open();
    }
    size_t matchA() const {
        return matchA_positions.size();
    }
    size_t matchB() const {
        return matchB_positions.size();
    }
    size_t print(bool all_flag=false, bool verbose_flag=false);
};

class Options {
    private:
    bool help_flag, list_flag, all_flag, case_flag;
    string target, folder, filename;
    chrono::time_point<chrono::high_resolution_clock> start_time; // Added for profiling

    Options(int argc, char *argv[]);

    public:
    Options(const Options&) = delete; // Prevent copying
    Options& operator=(const Options&) = delete; // Prevent assignment
    Options(Options&&) = delete; // Prevent moving
    Options& operator=(Options&&) = delete; // Prevent move assignment

    static Options& getInstance(int argc, char *argv[]) {
        static Options instance(argc, argv);
        return instance;
    }

    string getTarget() const { return target; }
    string getFolder() const { return folder; }
    string getFilename() const { return filename; }
    bool isList() const { return list_flag; }
    bool isAll() const { return all_flag; }
    bool isCase() const { return case_flag; }
    void help() const {
        Parrot parrot(FOLDER_STATIC "help.txt", "-h");
        if (!parrot.print()) {
            cerr << "Data in" FOLDER_STATIC "not accessible" << endl;
        }
    }
    ~Options() {
        // Destructor
        // Print execution time if profiling mode is enabled
        if (start_time.time_since_epoch().count() != 0) {
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
            cout << "\nExecution time: " << duration << " ms" << endl;
        }
        if (END_WITH_NEW_LINE)
            cout << endl;
    }
};

// Function to print the file content from a given position until a line matches the pattern or EOF
void print_matches(streampos pos, ifstream& file, regex pattern) {
    string line;
    string output;
    file.clear(); // Clear EOF flag
    file.seekg(pos); // Move to the matched position
    // Print file content until a line matches pattern or EOF
    while (getline(file, line) && !regex_match(line, pattern)) {
        output += line + "\n";
    }
    cout << output;
}

int main(int argc, char* argv[]) {
    Options* options;
    // Parse command line arguments
    try{
        options = &Options::getInstance(argc, argv);
    }
    catch (const std::runtime_error& e) {
        if (e.what() != std::string("Help requested") && e.what() != std::string("Folder path requested")) {
            cerr << "-h to get help" << endl;
            return 1;
        }
        return 0;
    }
    
    // No topic so we list all topics
    if (options->getFilename().empty()) {
        bool empty_folder = true;
        cout << "No topic provided.\nListing all existent ones in \"" << options->getFolder() << "\":" << endl;
        try {
            int i = 1;
            for (const auto& entry : fs::directory_iterator(options->getFolder())) {
                // Skip directories, hidden files, and non-txt files
                if (
                    entry.is_directory() ||
                    entry.path().filename().string().front() == '.' || 
                    is_extension(entry.path().filename().extension())
                ) {
                    continue;
                }
                empty_folder = false;
                cout << i << "    " << entry.path().stem().string() << endl;
                i++;
            }
        } catch (const fs::filesystem_error& e) {
            cerr << "Error accessing folder: " << e.what() << endl;
            return 1;
        }
        if (empty_folder) {
            cout << "No topics found" << endl;
            cout << "Create one yourself :)" << endl;
        }
        return 0;
    }
    // Looking into topic file
    else {
        // Looks for the target
        if (!options->getTarget().empty()) {
            Parrot parrot(options->getFilename());
            try {
                parrot.search(options->getTarget(), options->isCase());
            } catch (const std::runtime_error& e) {
                return 1;
            }
            if (parrot.print(options->isAll(), true) > 0) {
                cout << "Found " << parrot.matchA() + parrot.matchB() << " targets in " << options->getFilename() << endl;
            }
            else if (parrot.matchA() > 0 && !options->isAll()) {
                cout << "Found " << parrot.matchA() << " targets in " << options->getFilename() << endl;
                cout << "Use -a to print all targets" << endl;
            }
            else if (parrot.matchB() > 0 && !options->isAll()) {
                cout << "Found " << parrot.matchB() << " similar targets in " << options->getFilename() << endl;
                cout << "Use -a to print all similar targets" << endl;
            }
            else if (parrot.matchA() == 0 && parrot.matchB() == 0) {
                cout << "Target " << options->getTarget() << " not found in " << options->getFilename() << endl;
                cout << "Use -l to list all targets" << endl;
            }
        }
        // Print all targets
        else {
            // Check if the file exists
            string filepath = options->getFilename();
            if (!fs::exists(filepath)) {
                cerr << "File " << options->getFilename() << " does not exist" << endl;
                return 1;
            }
            // Open the file
            ifstream file(filepath);
            if (!file.is_open()) {
                cerr << "Failed to open file: " << filepath << endl;
                return 1;
            }
            // If you are here -l was set
            regex pattern(PATTERN);
            regex patternC(PATTERN_C);
            string line="";
            vector <string> targets;
            size_t i_target = 0;
            size_t i = 0;
            while (!file.eof()) {
                // at the first iteration line is empty
                // and doesnt match the pattern
                while (!file.eof() && regex_match(line, pattern)) {
                    // saves to targets all equivalent targets
                    auto begin = sregex_iterator(line.begin(), line.end(), patternC);
                    auto end = sregex_iterator();
                    for (auto it = begin; it != end; ++it) {
                        if (it->size() > 1) {
                            targets.push_back((*it)[1].str());
                        }
                    }
                    getline(file, line);
                }
                if (targets.size() > 0) {
                    cout << "Target " << i_target+1 << endl;
                    i_target ++;
                    // print all equivalent targets
                    cout << START_HIGHLIGHT;
                    for (i = 0; i < targets.size(); ++i) {
                        cout << targets[i] << endl;
                    }

                    cout << END_HIGHLIGHT;
                }
                // separate targets
                if (i){
                    cout << endl;
                    i = 0;
                }
                targets.clear();
                while (!file.eof() && !regex_match(line, pattern)){
                    getline(file, line);
                }
            }
            if (i_target) {
                cout << "\nFound " << i_target << " targets in " << options->getFilename() << endl;
            } 
            else {
                cout << "No targets found in " << options->getFilename() << endl;
                cout << "Create one yourself :)" << endl;
            }
            file.close();
        }
    }
    return 0;
}

Options::Options(int argc, char *argv[]): 
help_flag(false), list_flag(false), all_flag(false), case_flag(false),
target(""),
folder(std::filesystem::current_path().string() + "/" + FOLDER_DEFAULT),
filename(""),
start_time(chrono::time_point<chrono::high_resolution_clock>()) { // Initialize start_time to 0
    string topic = "";
    for (int i = 1; i < argc; ++i) {
        string arg_i = string(argv[i]);
        if (arg_i == "-h" or arg_i == "--help") {
            help_flag = true;
        }
        else if (arg_i.find("--folder=") == 0){
            folder = arg_i.substr(9);
            if (folder.back() != '/') {
                folder += '/';
            }
        }
        else if (arg_i.find("-F=") == 0) {
            folder = arg_i.substr(3);
            if (folder.back() != '/') {
                folder += '/';
            }
        }
        else if (arg_i == "-F" || arg_i == "--folder-path"){
            // print the folder path
            cout << std::filesystem::current_path().string() + "/" + folder << endl;
            throw std::runtime_error("Folder path requested");
        }
        else if (arg_i.find("--filename=") == 0) {
            filename = arg_i.substr(11);
        }
        else if (arg_i.find("-f=") == 0) {
            filename = arg_i.substr(3);
        }
        else if (arg_i == "-l" || arg_i == "--list") {
            // List all targets
            list_flag = true;
        }
        else if (arg_i == "-a" || arg_i == "--all") {
            // List all targets
            all_flag = true;
        }
        else if (arg_i == "-c" || arg_i == "--case-sensitive") {
            // Case sensitive search
            case_flag = true;
        }
        else if (arg_i == "-t" || arg_i == "--test-times") {
            // Profiling mode
            // Set start_time to now() if test flag is set
            start_time = chrono::high_resolution_clock::now(); 
        }
        else if (arg_i[0] == '-') {
            cerr << "Unknown option: " << arg_i << endl;
            throw std::runtime_error("Unknown option");
        }
        else if (topic.empty()) {
            topic = arg_i;
        }
        else if (target.empty()) {
            target = arg_i;
        }
    }

    if (filename.empty() && !topic.empty()) {
        // If no filename is provided, use the topic as the filename
        filename = folder + topic + implemented_extensions[0];
    }
    if (list_flag) {
        // If list_flag is set, all targets are listed
        target = "";
    }
    else if(!filename.empty() && target.empty()) {
        // Not gonna list all targets if not specified
        cerr << "No target specified" << endl;
        cerr << "Use -l to list all targets" << endl;
        throw std::runtime_error("No target specified");
    }
    if (all_flag && target.empty()) {
        // If all_flag is set, all matching targets are listed
        cerr << "-a option requires a target" << endl;
        cerr << "Use -l to list all targets" << endl;
        throw std::runtime_error("No target specified");
    }
    if (help_flag) {
        cout << "Help requested" << endl;
        help();
        throw std::runtime_error("Help requested");
    }
}

void Parrot::search(string target, bool case_flag) {
    // Constructor
    if (!fs::exists(filepath)) {
        cerr << "File " << filepath << " does not exist" << endl;
        throw std::runtime_error("File does not exist");
    }
    // Open the file
    file.open(filepath);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filepath << endl;
        throw std::runtime_error("Failed to open file");
    }
    regex pattern(PATTERN);  // match a target-like line
    regex patternA(
        PATTERN_A(target),
        (case_flag ? regex_constants::ECMAScript :  regex_constants::icase)
    );  // match the target
    regex patternB(PATTERN_B(target),
        (case_flag ? regex_constants::ECMAScript :  regex_constants::icase)
    );  // match a target like the target
    string line;
    smatch match;
    while (getline(file, line)) {
        if (regex_match(line, patternA)) {
            streampos pos = file.tellg();
            do {
                pos = file.tellg();
                getline(file, line);
            } while (!file.eof() && regex_match(line, pattern));
            matchA_positions.push_back(pos);
        }
        else if (regex_search(line, match, patternB) && match.size() > 1) {
            string matched_target = match[1].str();
            streampos pos = file.tellg();
            do {
                pos = file.tellg();
                getline(file, line);
            } while (!file.eof() && regex_match(line, pattern));
            matchB_positions.push_back(pos);
            matchB_targets.push_back(matched_target);
        }
    }
}

size_t Parrot::print(bool all_flag, bool verbose_flag) {
    // Print what was found
    // Giving priority to matchA
    // Return the number of matches printed
    if (!file.is_open()) {
        cerr << "File " << filepath << " is closed" << endl;
        throw std::runtime_error("File is closed");
    }
    regex pattern(PATTERN);  // match a target-like line
    if (matchA_positions.size() == 1) {
        // Print file content from the position until a line matches pattern or EOF
        print_matches(matchA_positions[0], file, pattern);
        return 1;
    }
    else if (matchA_positions.size() > 1 && all_flag) {
        if (verbose_flag) {
            cout << "Found " << matchA_positions.size() << " targets in " << filepath << endl;
        }
        for (size_t i = 0; i < matchA_positions.size(); ++i) {
            cout << (verbose_flag ? "Target " : "") << i + 1 << endl;
            print_matches(matchA_positions[i], file, pattern);
        }
        return matchA_positions.size();
    }
    else if (matchB_positions.size() == 1) {
        // Print file content from the position until a line matches pattern or EOF
        cout << (verbose_flag ? "Similar target found: " + HIGHLIGHT(matchB_targets[0]) + " in " + filepath : "") << endl;
        print_matches(matchB_positions[0], file, pattern);
        cout << endl;
        return 1;
    }
    else if (matchB_positions.size() > 1 && all_flag) {
        if (verbose_flag) {
            cout << "Found " << matchB_positions.size() << " similar targets in " << filepath << endl;
        }
        for (size_t i = 0; i < matchB_positions.size(); ++i) {
            cout << (verbose_flag ? "Similar target " : "") << i + 1 << ": " << HIGHLIGHT(matchB_targets[i]) << endl;
            print_matches(matchB_positions[i], file, pattern);
        }
        return matchB_positions.size();
    }
    else {
        return 0;
    }
}