//
//  main.cpp
//  proj3
//
//  Created by Kai Yamasaki on 3/10/23.
//
// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include <unordered_map>
#include <map>
#include <algorithm>
#include <iostream>
#include "TableEntry.h"
#include <string>
#include <vector>
#include <iostream>
#include <getopt.h>
#include <cstdio>


using namespace std;

inline void xcode_redirect(int argc, char **argv, int optind = 1) {
    while (optind < argc) {
        switch (*argv[optind]) {
            case '<':  // redirect input
                if (!freopen(argv[++optind], "r", stdin)) {
                    std::cerr << "Unable to open input file: " << argv[optind] <<
std::endl;
                    exit(1);
                }
                break;
            case '>':  // redirect output
                if (!freopen(argv[++optind], "w", stdout)) {
                    std::cerr << "Unable to open output file: " << argv[optind] <<
std::endl;
                    exit(1);
                }
                break;
        }  // switch
        ++optind;
    }  // while
}  // xcode_redirect()

void getMode(int argc, char * argv[], uint32_t &command) {
  // These are used with getopt_long()
  opterr = false; // Let us handle all error output for command line options
  int choice;
  int index = 0;
  option long_options[] = {
    // DONE: Fill in two lines, for the "mode" ('m') and
    // the "help" ('h') options.
    // ./project0 --mode nosize
    // ./project0 --help          //third argument always nullptr, cuz feature not used
      { "quiet", no_argument, nullptr, 'q' } ,
      { "help", no_argument, nullptr, 'h'} ,
    { nullptr, 0, nullptr, '\0' },
  };  // long_options[]

  // TODO: Fill in the double quotes, to match the mode and help options.
  while ((choice = getopt_long(argc, argv, "qh", long_options, &index)) != -1) {
      // : means m is followed by an argument, while h has no argument
    switch (choice) {
        case 'h': {  // Need a block here to declare a variable inside a case
            command = 1;
            break;
        }
        case 'q': {
            command = 2;
            break;
        }
      //default:
        //cerr << "Error: invalid option" << endl;
        //exit(1);
    }  // switch ..choice
  }  // while
}




/*
enum class CompType : uint8_t { Less, Greater, Equal };

struct EntryComp {
    template <typename T>
    bool operator()(const T  &lhs, const TableEntry &rhs, CompType comp) const {
        if (comp == CompType::Less) {
            return operator<(lhs, rhs);
        }
        else if (comp == CompType::Greater) {
            return operator>(lhs, rhs);
        }
        else {
            return operator==(lhs, rhs);
        }
        
    }
};
 */

struct Comparator {
    Comparator();
    Comparator(const char &comp, const uint32_t &num, const TableEntry &in) : compType(comp), col(num), compareTo(in) {}
    
    char compType;
    uint32_t col;
    TableEntry compareTo;
    
        
    bool operator()(const vector<TableEntry> &row) {
        //implement based on whichever operator you're doing
        if (compType == '<') {
            if (row[col] < compareTo) {
                return true;
            }
            return false;
        }
        else if (compType == '>') {
            if (row[col] > compareTo) {
                return true;
            }
            return false;
        }
        else {
            if (row[col] == compareTo) {
                return true;
            }
            return false;
        }
    }
    
};

class Table {
public:
    Table() {
        defaultEmpty = true;
    }
    
    Table(uint32_t num, vector<EntryType> types, vector<string> names) {
        colTypes = types;
        defaultEmpty = false;
        newTable.reserve(num);
        numCol = num;
        numRow = 0;
        unorderEXIST = false;
        regularEXIST = false;
        for (uint32_t i = 0; i < names.size(); ++i) {
            pair<string, uint32_t> one;
            one.first = names[i];
            one.second = i;
            colNames.insert(one);
        }
    }
    
    void insertRow() {
        //cout << numCol << endl << endl << endl;
        vector<TableEntry> input;
        input.reserve(numCol);
        
        for (uint32_t i = 0; i < numCol; ++i) {
            
            if (colTypes[i] == EntryType::String) {
                string x;
                cin >> x;
                TableEntry in(x);
                input.emplace_back(in);
            }
            else if (colTypes[i] == EntryType::Int) {
                int x;
                cin >> x;
                TableEntry in(x);
                input.emplace_back(in);
            }
            else if (colTypes[i] == EntryType::Double) {
                double x;
                cin >> x;
                TableEntry in(x);
                input.emplace_back(in);
            }
            else if (colTypes[i] == EntryType::Bool) {
                bool x;
                cin >> x;
                TableEntry in(x);
                input.emplace_back(in);
            }
            
        }
        newTable.emplace_back(input);
        ++numRow;
        
        if (regularEXIST) {
            auto infer = newTable[numRow - 1][mapCol];
            TableEntry here(infer);
            auto it = regMap.find(here);
            if (it != regMap.end()) {
                regMap.at(here).push_back(numRow - 1);
            }
            else {
                vector<uint32_t> boop;
                boop.push_back(numRow - 1);
                regMap.insert(pair<TableEntry, vector<uint32_t>>(here, boop));
            }
        }
        else if (unorderEXIST) {
            auto infer = newTable[numRow - 1][mapCol];
            TableEntry here(infer);
            auto it = unorderIndex.find(here);
            if (it != unorderIndex.end()) {
                unorderIndex.at(here).push_back(numRow - 1);
            }
            else {
                vector<uint32_t> boop;
                boop.push_back(numRow - 1);
                unorderIndex.insert(pair<TableEntry, vector<uint32_t>>(here, boop));
            }
        }
    }
    
    
    void print(uint32_t &numToPrint, string &TName, bool &quiet) {
        //cout << numCol << endl << endl << endl;
        
        vector<uint32_t> indexes;
        indexes.reserve(numToPrint);
        string blank;
        vector<string> stringsToPrint;
        for (uint32_t i = 0; i < numToPrint; ++i)  {
            cin >> blank;
            auto it = colNames.find(blank);
            if (it == colNames.end()) {
                cout << "Error during PRINT: " << blank << " does not name a column in " << TName << endl;
                getline(cin, TName);
                return;
            }
            indexes.emplace_back(colNames[blank]);
            stringsToPrint.push_back(blank);
        }
        cin >> blank;
        //cout << blank << endl;
        if (blank == "ALL") {
            if (!quiet) {
                for (uint32_t j = 0; j < stringsToPrint.size(); ++j) {
                    cout << stringsToPrint[j] << " ";
                }
                cout << endl;
                for (uint32_t i = 0; i < numRow; ++i) {
                    for (uint32_t j = 0 ; j < numToPrint; ++j) {
                        cout << newTable[i][indexes[j]] << " ";
                    }
                    cout << endl;
                }
            }
            
            
            cout << "Printed " << numRow << " matching rows from " << TName << endl;
            
            // TODO: CUT THIS OUT BELOW
            /*
            if (numRow == 174 && TName == "sexy") {
                int x = 4;
            }
             */
        }
        else {
            // WHERE y/n = true
            //blank is the col name, where is in the previous blank
            char comparator;
            string condition;
            cin >> blank >> comparator;
            
            uint32_t indexToCheck;
            auto it = colNames.find(blank);
            if (it == colNames.end()) {
                cout << "Error during PRINT: " << blank << " does not name a column in " << TName << endl;
                getline(cin, TName);
                return;
            }
            else {
                if (!quiet) {
                    for (uint32_t j = 0; j < stringsToPrint.size(); ++j) {
                        cout << stringsToPrint[j] << " ";
                    }
                    cout << endl;
                }
                indexToCheck = colNames[blank];
            }
            //CHECK THIS HERE
            Comparator compie(comparator, indexToCheck, getColumnEntry(indexToCheck, colTypes[indexToCheck]));
            
            
            // TODO: CUT THIS OUT BELOW
            /*
            if (mapCol == 23 && numCol == 70 && numRow == 174 && TName == "sexy") {
                
                for (uint32_t i = 0; i < numRow; ++i) {
                    for (uint32_t j = 0 ; j < numToPrint; ++j) {
                        cout << newTable[i][indexes[j]] << " ";
                    }
                    cout << endl;
                }
                 
                cout << endl << endl << endl << endl;
                
            }
             */
            
            
            
            
            if (regularEXIST && mapCol == indexToCheck) {
                int count = 0;
                for (auto const& rn : regMap) {
                    if (compie(newTable[rn.second[0]])) {
                        for (uint32_t i = 0 ; i < rn.second.size(); ++i) {
                            for (uint32_t j = 0 ; j < numToPrint; ++j) {
                                if (!quiet) {
                                    cout << newTable[rn.second[i]][indexes[j]] << " ";
                                }
                            }
                            count++;
                            if (!quiet) {
                                cout << endl;
                            }
                            
                            //cout << newTable[i][indexes[j]] << " ";
                            //count++:
                        }
                    }
                }
                cout << "Printed " << count << " matching rows from " << TName << endl;
                /*
                if (count == 174 && TName == "sexy") {
                    int x = 4;
                }
                 */
            }
            else {
                uint32_t count = 0;
                for (uint32_t i = 0; i < numRow; ++i) {
                    if (compie(newTable[i])) {
                        count++;
                        for (uint32_t j = 0 ; j < numToPrint; ++j) {
                            if (!quiet) {
                                cout << newTable[i][indexes[j]] << " ";
                            }
                        }
                        if (!quiet) {
                            cout << endl;
                        }
                    }
                }
                cout << "Printed " << count << " matching rows from " << TName << endl;
            }
        }
    }
    
    void deleteOP(string &TName) {
        //cout << numCol << endl << endl << endl;
        
        string checker;
        char comparator;
        string condition;
        cin >> checker >> comparator;
        auto it = colNames.find(checker);
        if (it == colNames.end()) {
            cout << "Error during DELETE: " << checker << " does not name a column in " << TName << endl;
            getline(cin, TName);
        }
        else {
            uint32_t indexToCheck = colNames[checker];
            Comparator compie(comparator, indexToCheck, getColumnEntry(indexToCheck, colTypes[indexToCheck]));

            
            vector<vector<TableEntry>>::iterator it1 = newTable.end();
            //int differs = newTable.end() - newTable.begin();
            auto it2 = remove_if(newTable.begin(), newTable.end(), compie);
            //int differs2 = newTable.end() - it2;
            
            uint32_t numDeleted = static_cast<uint32_t>(abs(it1 - it2));
            newTable.erase(it2, it1);
            //MAYBE add a resize here?
            //Deleted 2 rows from 281class
            cout << "Deleted " << numDeleted << " rows from " << TName << endl;
            numRow = numRow - numDeleted;
        }
        checker = "";
        
        if (regularEXIST) {
            generateIndex('b', /*checker,*/ mapCol);
        }
        else if (unorderEXIST) {
            generateIndex('h', /*checker, */ mapCol);
        }
    }
    
    TableEntry getColumnEntry(uint32_t indexToCheck, EntryType &typeCheck) {
        if(typeCheck !=  colTypes[indexToCheck]) {
            cerr << "Error: incorrect type\n";
            exit(1);
        }
     switch (colTypes[indexToCheck]) {
         case EntryType::Int: {
             int x;
             cin >> x;
             TableEntry bar(x);
             return bar;
             break;
         }
         case EntryType::Double: {
             double x;
             cin >> x;
             TableEntry bar(x);
             return bar;
             break;
         }
         case EntryType::Bool: {
             bool x;
             cin >> x;
             TableEntry bar(x);
             return bar;
             break;
         }
         case EntryType::String: {
             string x;
             cin >> x;
             TableEntry bar(x);
             return bar;
             break;
         }
         default:
             cerr << "Error: incorrect type\n";
             exit(1);
             break;
        }
    }
    
    void joinNow(Table &tableTwo, string& col1Name, string &col2Name, vector<pair<uint32_t, uint32_t>>& reqPrints, string desired, bool &quiet, vector<string> &readers) {
        uint32_t oneCheck = getIndexFromCName(col1Name, true, desired/*,tableTwo, desired*/);
        if (static_cast<int>(oneCheck) == -1) {
            return;
        }
        //uint32_t twoCheck = tableTwo.getIndexFromCName(col2Name);
        //cout << numCol << endl << endl << endl;
        
        char h = 'h';
        bool isHash = true;
        string blank = "";
        /*
        for (int i = 0; i < reqPrints.size(); ++i) {
            cout << reqPrints[i].first << " and " << reqPrints[i].second << endl;
        }
         */
       // cout << "COLS ONE: " << col1Name << " TWO: " << col2Name << endl << endl;
        uint32_t booze = tableTwo.getIndexFromCName(col2Name, true, desired);
        if (static_cast<int>(booze) == -1) {
            return;
        }
        else if (!quiet) {
            for (uint32_t k = 0; k < readers.size(); ++k) {
                cout << readers[k] << " ";
            }
            cout << endl;
        }
        tableTwo.generateIndex(h, booze);
        size_t count = 0;
        for (uint32_t i = 0; i < numRow; ++i) {
            vector<uint32_t> matches = tableTwo.getGenIndex(isHash, newTable[i][oneCheck]);
            //for (int i  = 0; i < matches.size(); ++i) {
              //  cout << matches[i] << " ";
                
            //}
            if (!quiet) {
                for (uint32_t m = 0; m < matches.size(); ++m) {
                    for (uint32_t j = 0; j < reqPrints.size(); ++j) {
                        switch (reqPrints[j].first) {
                            case 1:
                                cout << newTable[i][reqPrints[j].second] << " ";
                                break;
                            case 2:
                                //cout << i << " " << reqPrints[j].second << endl;
                                cout << tableTwo.newTable[matches[m]][reqPrints[j].second] << " ";
                                break;
                                
                            default:
                                cerr << "ERROR: invalid table num requested\n";
                                exit(1);
                                break;
                        }
                    }
                    cout << endl;
                }
            }
            count = count + matches.size();
            //Printed 3 rows from joining pets to 281class
        }
        cout << "Printed " << count << " rows from joining ";
    }
    
    vector<uint32_t> &getGenIndex(bool isHash, TableEntry checker) {
        static vector<uint32_t> blank;
        if (isHash) {
            auto it = unorderIndex.find(checker);
            if (it != unorderIndex.end()) {
                return unorderIndex.at(checker);
            }
        }
        else {
            auto it = regMap.find(checker);
            if (it != regMap.end()) {
                return regMap.at(checker);
            }
        }
        return blank;
    }
    
    void generateIndex(char indexType, /*string &TName,*/ uint32_t inters) {
        //cout << numCol << endl << endl << endl;
        
        if (unorderEXIST) {
            unorderIndex.clear();
            unorderEXIST = false;
        }
        if (regularEXIST) {
            regMap.clear();
            regularEXIST = false;
        }
       
        //auto it = colNames.find(blank);
        //if (it == colNames.end()) {
          ///  cerr << "Error during GENERATE: " << blank << " does not name a column in P" << TName << endl;
            //exit(1);
       // }
        mapCol = inters;
        
        switch (indexType) {
            case 'h':
                //unorderIndex = new unordered_map<TableEntry, vector<uint32_t>>();
                for (uint32_t i = 0; i < numRow; ++i) {
                    auto infer = newTable[i][mapCol];
                    TableEntry here(infer);
                    auto it = unorderIndex.find(here);
                    if (it == unorderIndex.end()) {
                        vector<uint32_t> boop;
                        boop.push_back(i);
                        unorderIndex.insert(pair<TableEntry, vector<uint32_t>>(here, boop));
                    }
                    else {
                        unorderIndex.at(here).push_back(i);
                    }
                    //reated hash index for table 281class on column emotion
                }
                unorderEXIST = true;
                regularEXIST = false;
               
                break;
            case 'b':
                //regMap = new map<TableEntry, vector<uint32_t>>();
                for (uint32_t i = 0; i < numRow; ++i) {
                    auto infer = newTable[i][mapCol];
                    TableEntry here(infer);
                    auto it = regMap.find(here);
                    if (it == regMap.end()) {
                        vector<uint32_t> boop;
                        boop.push_back(i);
                        regMap.insert(pair<TableEntry, vector<uint32_t>>(here, boop));
                    }
                    else {
                        regMap.at(here).push_back(i);
                    }
                }
                
                regularEXIST = true;
                unorderEXIST = false; 
                break;
            default:
                cout << "Error: incorrect type\n";
                exit(1);
                break;
        }
        
    }
    
    
    
    uint32_t getNumRows() const {
        return numRow;
    }
    
    void reserve(uint32_t x) {
        newTable.reserve(x);
    }
    
  
        
    
    uint32_t getIndexFromCName(string here, bool isJoin/*,Table &tubby*/, string FINALLY) const {
        auto it = colNames.find(here);
        /*
        if (here == "honest") {
            cout << numCol << endl;
            for (const auto& [key, value] : colNames) {
                cout << key << endl;
            }
            
        }
         */
        
        
        if (it != colNames.end()) {
            return colNames.at(here);
        }
        // TODO: CUT THIS OUT BELOW
        /*
        
        cout << endl << endl << endl << endl << endl;
        cout << "WORD: " << here << endl << endl << endl;
        cout << "TNAME: " << FINALLY << endl << endl << endl;
        
        
        
        
        for (const auto& [key, value] : colNames) {
            cout << key << endl;
        }
        
        
            
        int x  = tubby.numRow;
        */
        
        //rror during <COMMAND>: <colname> does not name a column in <tablename>
        if (isJoin) {
            cout << "Error during JOIN: " << here << " does not name a column in " << FINALLY << endl;
            getline(cin, FINALLY);
            uint32_t oops = static_cast<uint32_t>(-1);
            return oops;
        }
        else {
            cout << "Error during GENERATE: " << here << " does not name a column in " << FINALLY << endl;
            getline(cin, FINALLY);
            uint32_t oops = static_cast<uint32_t>(-1);
            return oops;
        }

         
       
    }
    
    
    
private:
    uint32_t mapCol;
    bool unorderEXIST;
    bool regularEXIST;
    unordered_map<TableEntry, vector<uint32_t>> unorderIndex;
    map<TableEntry, vector<uint32_t>> regMap;
    bool defaultEmpty;
    uint32_t numCol;
    uint32_t numRow;
    vector<EntryType> colTypes;
    unordered_map<string, uint32_t> colNames;
    vector<vector<TableEntry>> newTable;
};

vector<EntryType> getEntryTypes(uint32_t number) {
    vector<EntryType> types;
    types.resize(number);
    string in;
    for (uint32_t i = 0; i < number; i++) {
        cin >> in;
        if (in == "string") {
            types[i] = EntryType::String;
        }
        else if (in == "int") {
            types[i] = EntryType::Int;
        }
        else if (in == "double") {
            types[i] = EntryType::Double;
        }
        else if (in == "bool") {
            types[i] = EntryType::Bool;
        }
        else {
            cout << "ERROR: UNKNOWN TYPE\n";
            exit(1);
        }
    }
    return types;
}
vector<string> getEntryNames(uint32_t number) {
    string in;
    vector<string> names;
    names.resize(number);
    for (uint32_t i = 0; i < number; ++i) {
        cin >> in;
        names[i] =  in;
    }
    return names;
}

int main(int argc, char * argv[]) {
    xcode_redirect(argc, argv);
    //might not need this?
    
    
    //TODO: CODE HELP AND QUIET COMMANDS
    
    ios_base::sync_with_stdio(false); // you should already have this
    cin >> std::boolalpha;  // add these two lines
    cout << std::boolalpha; // add these two lines
        
    vector<int> vec;
    vec.resize(5);
        // the rest of your program
    string command;
    unordered_map<string, Table> hashTab;
    uint32_t cLine = 0;
    getMode(argc, argv, cLine);
    //CLINE
    // 1 for quiet, 2 for help
    bool quiet = false;
    if (cLine == 1) {
        cout << "helpful message about how to use the program\n";
        exit(0);
    }
    else if (cLine == 2) {
        quiet = true;
    }
    
    //int count = 0;
    //int boomer = 1;


    //bool justCleared = false;
    int bigboom = 0;
    do {
       
            cout << "% ";
    
        cin >> command;
        bigboom++;
        
        /*
        if (command == "INSERT" && bigboom == 2) {
            string blank;
            getline(cin, blank);
            cout << endl << blank << endl;
            
            
        }
         */
        
    
       
        if (command[0] == '#') {
            getline(cin, command);
        }
        else if (command == "CREATE") {
            string name;
            cin >> name;
            uint32_t number;
            cin >> number;
            vector<EntryType> types;
            vector<string> names;
            
            types = getEntryTypes(number);
            names = getEntryNames(number);
            
            auto it = hashTab.find(name);
            if (it == hashTab.end()) {
                Table here(number, types, names);
                hashTab.insert(make_pair(name, here));
                cout << "New table " << name << " with column(s) ";
                for (uint32_t i = 0; i < number; ++i) {
                    cout << names[i] << " ";
                }
                cout << "created\n";
            }
            else {
                cout << "ERROR during CREATE: Cannot create already existing table " << name << endl;
                getline(cin, name);
            }

        }
        else if (command == "REMOVE") {
            string name;
            cin >> name;
            auto it = hashTab.find(name);
            if (it != hashTab.end()) {
                hashTab.erase(it);
                cout << "Table " << name << " deleted\n";
            }
            else {
                //Error during <COMMAND>: <tablename> does not name a table in the database
                cout << "Error during REMOVE: " << name << " does not name a table in the database\n";
                getline(cin, name);
            }
        }
        else if (command == "INSERT") {
            string blank;
            string TName;
            uint32_t newRows;
            cin >> blank >> TName;
            auto it = hashTab.find(TName);
            if (it == hashTab.end()) {
                cout << "Error during INSERT: " << TName << " does not name a table in the database\n";
                cin >> newRows;
                getline(cin, TName);
               
                //insertJunk = newRows;
            }
            else {
                cin >> newRows >> blank;
                uint32_t x = hashTab[TName].getNumRows();
                hashTab[TName].reserve(x + newRows);
                for (uint32_t i = 0; i < newRows; ++i) {
                    hashTab[TName].insertRow();
                }
                //Added <N> rows to <tablename> from position <startN> to <endN>
                cout << "Added " << newRows << " rows to " << TName << " from position "
                << x << " to " << hashTab[TName].getNumRows() - 1 << endl;
            }
            
        }
        else if (command == "PRINT") {
            /*
            if (boomer == 6) {
                int x = 4;
            }
            ++boomer;
             */
            string blank;
            string TName;
            uint32_t printMRows;
            cin >> blank >> TName >> printMRows;
            auto it = hashTab.find(TName);
            if (it != hashTab.end()) {
                hashTab[TName].print(printMRows, TName, quiet);
            }
            else {
                //Error during <COMMAND>: <tablename> does not name a table in the database
                cout << "Error during PRINT: " << TName << " does not name a table in the database\n";
                getline(cin, TName);
                
            }
          
            
           
        }
        else if (command == "DELETE") {
            string blank;
            string TName;
            cin >> blank >> TName >> blank;
            auto it = hashTab.find(TName);
            if (it != hashTab.end()) {
                hashTab[TName].deleteOP(TName);
            }
            else {
                //Error during <COMMAND>: <tablename> does not name a table in the database
                cout << "Error during DELETE: " << TName << " does not name a table in the database\n";
                getline(cin, TName);
                
            }
            
        }
        else if (command == "JOIN") {
            string blank;
            string colName1;
            string colName2;
            string nameOne;
            string nameTwo;
            uint32_t numPrint;
            
            cin >> nameOne >> blank >> nameTwo >> blank >> colName1 >> blank >> colName2 >> blank >> blank >> numPrint;
            
            vector<pair<uint32_t, uint32_t>> reqPrints;
            //bool is true for table 1, false for 2
            string read;
            uint32_t OneORTwo;
            uint32_t index;
            
            // TODO: CUT THIS OUT BELOW
            /*
            if (nameOne == "sick" && nameTwo == "partnership" && colName1 == "hurt" && colName2 == "honest") {
                int z = 4;
            }
             */
            auto it = hashTab.find(nameOne);
            auto it2 = hashTab.find(nameTwo);
            if (it == hashTab.end()) {
                cout << "Error during JOIN: " << nameOne << " does not name a table in the database\n";
                getline(cin, nameOne);
            }
            else if (it2 == hashTab.end()) {
                cout << "Error during JOIN: " << nameTwo << " does not name a table in the database\n";
                getline(cin, nameTwo);
            }
            else {
                auto booze = hashTab[nameOne].getIndexFromCName(colName1, true, nameOne);
                if (static_cast<int>(booze) == -1) {
                    
                }
                else {
                    auto booze2 = hashTab[nameTwo].getIndexFromCName(colName2, true, nameTwo);
                    if(static_cast<int>(booze2) == -1) {
                        
                    }
                    else {
                        vector<string> readers;
                        bool broken = false;
                        for (uint32_t i = 0; i < numPrint; ++i) {
                            if (broken == true) {
                                break;
                            }
                            cin >> read >> OneORTwo;
                            switch (OneORTwo) {
                                case 1:
                                    index = hashTab[nameOne].getIndexFromCName(read, true, nameOne/*, hashTab[nameOne], nameOne*/);
                                    if (static_cast<int>(index) == -1) {
                                        broken = true;
                                        readers.push_back(read);
                                        break;
                                        
                                    }
                                    /*
                                    if (!quiet) {
                                        cout << read << " ";
                                    }
                                     */
                                    readers.push_back(read);
                                    break;
                                case 2:
                                    index = hashTab[nameTwo].getIndexFromCName(read, true, nameTwo/*, hashTab[nameTwo], nameTwo*/);
                                    if (static_cast<int>(index) == -1) {
                                        broken = true;
                                        readers.push_back(read);
                                        break;
                                    }
                                    /*
                                    if (!quiet) {
                                        cout << read << " ";
                                    }
                                    */
                                    readers.push_back(read);
                                    break;
                                default:
                                    cout << "ERROR: invalid table num requested\n";
                                    exit(1);
                                    break;
                            }
                            //cout << index << endl;
                            pair<uint32_t, uint32_t> in(OneORTwo, index);
                            reqPrints.push_back(in);
                        }
                        if (static_cast<int>(booze) == -1) {
                            
                        }
                        else if (static_cast<int>(booze2) == -1) {
                            
                        }
                        else if (!broken) {
                            
                            //cout << endl << endl << "HTAB1: " << nameOne << " " << "HTAB2: " << nameTwo << endl << endl;
                            hashTab[nameOne].joinNow(hashTab[nameTwo], colName1, colName2, reqPrints, nameTwo, quiet, readers);
                            cout << nameOne << " to " << nameTwo << endl;
                        }
                        /*
                        else if (isAvatar == false) {
                            cout << "Error during JOIN: " << readers[readers.size() - 1] << " does not name a column in " << nameOne << endl;
                            getline(cin, nameTwo);
                        }
                        else {
                            cout << "Error during JOIN: " << readers[readers.size() - 1] << " does not name a column in " << nameTwo << endl;
                            getline(cin, nameTwo);
                        }
                         */
                    }
                }
              
            }
            
            
        }
        else if (command == "GENERATE") {
            // TODO:
            string blank;
            
            string TName;
            string indexType;
            cin >> TName >> TName >> indexType >> blank >> blank >> blank;
            auto it = hashTab.find(TName);
            if (it != hashTab.end()) {
                //cout << TName << endl;
                /*
                if (indexType[0] == 'b' && TName == "sexy" && blank == "start") {
                    int q = 4;
                }
                 */
                uint32_t inters = hashTab[TName].getIndexFromCName(blank, false, TName/*, hashTab[TName], TName*/);
                if (static_cast<int>(inters) != -1) {
                    hashTab[TName].generateIndex(indexType[0], /*TName, */ inters);
                    
                    cout << "Created " << indexType << " index for table " << TName << " on column " << blank << endl;
                }
                
            }
            else {
                //Error during <COMMAND>: <tablename> does not name a table in the database
                cout << "Error during GENERATE: " << TName << " does not name a table in the database\n";
                getline(cin, TName);
            }
        }
        else if (command == "QUIT") {
            break;
        }
        else {/*
            if (clearInsertData) {
                for (uint32_t i = 0; i < insertJunk; ++i) {
                    getline(cin, command);
                }
                clearInsertData = false;
                insertJunk = 0;
                justCleared = true;
            }
               */
            //else {
                cout << "Error: unrecognized command\n";
                getline(cin, command);
            //}
        }
        
    } while (command != "QUIT");
    cout << "Thanks for being silly!\n";
    
    return 0;
}



/*
 class Table {
 public:
     // Constructor to initialize the column types
     Table(vector<TableEntry> types) {
         columnTypes = types;
     }

     // Method to add a new row
     void addRow(vector<TableEntry> data) {
         // Check that the row has the same number of columns as the columnTypes vector
         if (data.size() != columnTypes.size()) {
             throw runtime_error("Row has incorrect number of columns");
         }

         // Add the row to the table
         rows.push_back(data);
     }

     // Method to get the number of rows in the table
     int numRows() const {
         return rows.size();
     }

     // Method to get the number of columns in the table
     int numCols() const {
         return columnTypes.size();
     }

     // Method to get the type of a column
     TableEntry columnType(int col) const {
         if (col < 0 || col >= columnTypes.size()) {
             throw runtime_error("Invalid column index");
         }
         return columnTypes[col];
     }

     // Method to get the data in a cell
     TableEntry cell(int row, int col) const {
         if (row < 0 || row >= rows.size()) {
             throw runtime_error("Invalid row index");
         }
         if (col < 0 || col >= columnTypes.size()) {
             throw runtime_error("Invalid column index");
         }
         return rows[row][col];
     }

 private:
     vector<TableEntry> columnTypes; // Vector to store the column types
     vector<vector<TableEntry>> rows; // Vector to store the data for each row
 };
 
 
 
 */
