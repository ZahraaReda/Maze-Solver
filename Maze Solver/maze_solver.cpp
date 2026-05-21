/*
 * ================================================================
 *  Maze Solver — Algorithms Final Project
 *  C++ Implementation with Console Frontend
 *
 *  Team Members:
 *    Jana Ahmed Elzamzamy   — 2401507
 *    Esraa Mohsen Omar      — 2401532
 *    Zahraa Reda Mohamed    — 2401422
 *    Alaa Ali Mohamed       — 2401569
 *
 *  Submitted to: Eng. Katreen Essam
 *  Course: Algorithms — Faculty of Computers and AI
 * ================================================================
 *
 *  Algorithms: DFS (Recursive + Iterative)  &  BFS
 *  Maze characters:
 *    '#'  → Wall
 *    ' '  → Passable cell
 *    'S'  → Start
 *    'E'  → Exit
 *    '*'  → Solution path (displayed after solving)
 * ================================================================
 */

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <string>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <limits>

using namespace std;

// ─────────────────────────────────────────────
//  ANSI colour helpers (Windows-safe fallback)
// ─────────────────────────────────────────────
#ifdef _WIN32
  #define CLR_RESET  ""
  #define CLR_WALL   ""
  #define CLR_PATH   ""
  #define CLR_START  ""
  #define CLR_END    ""
  #define CLR_SOLVE  ""
  #define CLR_TITLE  ""
  #define CLR_HEAD   ""
  #define CLR_INFO   ""
  #define CLR_WARN   ""
  #define CLR_OK     ""
#else
  #define CLR_RESET  "\033[0m"
  #define CLR_WALL   "\033[90m"      // dark grey
  #define CLR_PATH   "\033[37m"      // white
  #define CLR_START  "\033[1;32m"    // bold green
  #define CLR_END    "\033[1;31m"    // bold red
  #define CLR_SOLVE  "\033[1;33m"    // bold yellow
  #define CLR_TITLE  "\033[1;36m"    // bold cyan
  #define CLR_HEAD   "\033[1;35m"    // bold magenta
  #define CLR_INFO   "\033[36m"      // cyan
  #define CLR_WARN   "\033[33m"      // yellow
  #define CLR_OK     "\033[32m"      // green
#endif

// ─────────────────────────────────────────────
//  Direction arrays (Down, Up, Right, Left)
// ─────────────────────────────────────────────
const int DR[] = {1, -1, 0,  0};
const int DC[] = {0,  0, 1, -1};
const string DIR_NAME[] = {"Down","Up","Right","Left"};

// ─────────────────────────────────────────────
//  Utility: print horizontal separator
// ─────────────────────────────────────────────
void separator(char c = '=', int len = 60) {
    cout << CLR_INFO;
    for (int i = 0; i < len; i++) cout << c;
    cout << CLR_RESET << "\n";
}

// ─────────────────────────────────────────────
//  Maze printing with colours
// ─────────────────────────────────────────────
void printMaze(const vector<string>& maze,
               const vector<pair<int,int>>& highlight = {}) {
    // Build a highlight set for O(1) lookup
    map<pair<int,int>, bool> hl;
    for (auto& p : highlight) hl[p] = true;

    for (int r = 0; r < (int)maze.size(); r++) {
        cout << "  ";
        for (int c = 0; c < (int)maze[r].size(); c++) {
            char ch = maze[r][c];
            if (ch == '#') {
                cout << CLR_WALL << "##" << CLR_RESET;
            } else if (ch == 'S') {
                cout << CLR_START << " S" << CLR_RESET;
            } else if (ch == 'E') {
                cout << CLR_END << " E" << CLR_RESET;
            } else if (hl.count({r,c})) {
                cout << CLR_SOLVE << " *" << CLR_RESET;
            } else {
                cout << CLR_PATH << "  " << CLR_RESET;
            }
        }
        cout << "\n";
    }
}

// ─────────────────────────────────────────────
//  Mark solution path in maze copy and print
// ─────────────────────────────────────────────
void visualize(const vector<string>& maze,
               const vector<pair<int,int>>& path,
               const string& label) {
    vector<string> copy = maze;
    for (auto& [r, c] : path)
        if (copy[r][c] != 'S' && copy[r][c] != 'E')
            copy[r][c] = '*';

    cout << "\n" << CLR_HEAD << "  [" << label << "] Solution Path:\n" << CLR_RESET;
    for (auto& row : copy) {
        cout << "  ";
        for (char ch : row) {
            if (ch == '#')        cout << CLR_WALL   << "##" << CLR_RESET;
            else if (ch == 'S')   cout << CLR_START  << " S" << CLR_RESET;
            else if (ch == 'E')   cout << CLR_END    << " E" << CLR_RESET;
            else if (ch == '*')   cout << CLR_SOLVE  << " *" << CLR_RESET;
            else                  cout << CLR_PATH   << "  " << CLR_RESET;
        }
        cout << "\n";
    }
}

// ─────────────────────────────────────────────
//  Bounds & wall check helper
// ─────────────────────────────────────────────
inline bool valid(const vector<string>& maze,
                  vector<vector<bool>>& visited,
                  int r, int c) {
    int rows = maze.size(), cols = maze[0].size();
    return r >= 0 && r < rows && c >= 0 && c < cols
           && maze[r][c] != '#'
           && !visited[r][c];
}

// ══════════════════════════════════════════════════════════════
//  ALGORITHM 1 — DFS (Recursive)
//
//  Time  Complexity : O(V + E)   V = cells, E = passable edges
//  Space Complexity : O(V)       call stack depth ≤ V
//
//  Returns true when exit found, accumulating path via backtracking
// ══════════════════════════════════════════════════════════════
bool dfsRecursive(const vector<string>& maze,
                  vector<vector<bool>>& visited,
                  int r, int c,
                  int endR, int endC,
                  vector<pair<int,int>>& path) {
    // Bounds, wall, or already-visited check
    if (r < 0 || r >= (int)maze.size() ||
        c < 0 || c >= (int)maze[0].size()) return false;
    if (maze[r][c] == '#' || visited[r][c])  return false;

    visited[r][c] = true;
    path.push_back({r, c});

    // Goal reached
    if (r == endR && c == endC) return true;

    // Recurse in 4 directions
    for (int d = 0; d < 4; d++) {
        if (dfsRecursive(maze, visited,
                         r + DR[d], c + DC[d],
                         endR, endC, path))
            return true;
    }

    path.pop_back();   // Backtrack
    return false;
}

// ══════════════════════════════════════════════════════════════
//  ALGORITHM 2 — DFS (Iterative, explicit stack)
//
//  Time  Complexity : O(V + E)
//  Space Complexity : O(V)   explicit stack ≤ V elements
//
//  Uses a stack of (cell, path) pairs to avoid recursion limit
// ══════════════════════════════════════════════════════════════
vector<pair<int,int>> dfsIterative(const vector<string>& maze,
                                   int startR, int startC,
                                   int endR,   int endC) {
    int rows = maze.size(), cols = maze[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));

    // Stack stores (row, col) + current path to that cell
    stack<pair<pair<int,int>, vector<pair<int,int>>>> stk;
    stk.push({{startR, startC}, {{startR, startC}}});
    visited[startR][startC] = true;

    while (!stk.empty()) {
        auto [cell, path] = stk.top();
        stk.pop();
        auto [r, c] = cell;

        if (r == endR && c == endC) return path;

        // Push neighbours in reverse order so Down is tried first
        for (int d = 3; d >= 0; d--) {
            int nr = r + DR[d], nc = c + DC[d];
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols
                && maze[nr][nc] != '#' && !visited[nr][nc]) {
                visited[nr][nc] = true;
                auto np = path;
                np.push_back({nr, nc});
                stk.push({{nr, nc}, np});
            }
        }
    }
    return {};   // No path found
}

// ══════════════════════════════════════════════════════════════
//  ALGORITHM 3 — BFS (Guarantees shortest path)
//
//  Time  Complexity : O(V + E)
//  Space Complexity : O(V)   queue + visited + parent map
//
//  Explores cells level-by-level; first time E is reached
//  it is via the shortest possible route.
// ══════════════════════════════════════════════════════════════
vector<pair<int,int>> bfs(const vector<string>& maze,
                          int startR, int startC,
                          int endR,   int endC) {
    int rows = maze.size(), cols = maze[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    map<pair<int,int>, pair<int,int>> parent;   // child → parent

    queue<pair<int,int>> q;
    q.push({startR, startC});
    visited[startR][startC] = true;

    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();

        if (r == endR && c == endC) {
            // Reconstruct path by walking parent pointers
            vector<pair<int,int>> path;
            for (auto cur = make_pair(endR, endC);
                 cur != make_pair(startR, startC);
                 cur = parent[cur])
                path.push_back(cur);
            path.push_back({startR, startC});
            reverse(path.begin(), path.end());
            return path;
        }

        for (int d = 0; d < 4; d++) {
            int nr = r + DR[d], nc = c + DC[d];
            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols
                && maze[nr][nc] != '#' && !visited[nr][nc]) {
                visited[nr][nc] = true;
                parent[{nr, nc}] = {r, c};
                q.push({nr, nc});
            }
        }
    }
    return {};   // No path found
}

// ─────────────────────────────────────────────
//  Locate a character in the maze
// ─────────────────────────────────────────────
pair<int,int> findCell(const vector<string>& maze, char target) {
    for (int r = 0; r < (int)maze.size(); r++)
        for (int c = 0; c < (int)maze[r].size(); c++)
            if (maze[r][c] == target)
                return {r, c};
    return {-1, -1};
}

// ─────────────────────────────────────────────
//  Run one algorithm, time it, and display result
// ─────────────────────────────────────────────
void runAlgorithm(const string& name,
                  const vector<string>& maze,
                  int startR, int startC,
                  int endR,   int endC,
                  int algo) {   // 0=DFS-R, 1=DFS-I, 2=BFS

    separator('-', 60);
    cout << CLR_HEAD << "  Algorithm: " << name << CLR_RESET << "\n";
    separator('-', 60);

    auto t0 = chrono::high_resolution_clock::now();

    vector<pair<int,int>> path;

    if (algo == 0) {
        // DFS Recursive
        int rows = maze.size(), cols = maze[0].size();
        vector<vector<bool>> vis(rows, vector<bool>(cols, false));
        bool found = dfsRecursive(maze, vis, startR, startC, endR, endC, path);
        if (!found) path.clear();

    } else if (algo == 1) {
        // DFS Iterative
        path = dfsIterative(maze, startR, startC, endR, endC);

    } else {
        // BFS
        path = bfs(maze, startR, startC, endR, endC);
    }

    auto t1 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t1 - t0).count();

    if (path.empty()) {
        cout << CLR_WARN << "  ✗ No path found!\n" << CLR_RESET;
    } else {
        visualize(maze, path, name);
        cout << "\n";
        cout << CLR_OK << "  ✓ Path found!\n" << CLR_RESET;
        cout << "  Path length   : " << CLR_INFO << path.size()
             << " cells" << CLR_RESET << "\n";
        cout << "  Elapsed time  : " << CLR_INFO << fixed
             << setprecision(4) << ms << " ms" << CLR_RESET << "\n";

        // Print step-by-step direction log
        cout << "  Steps         : ";
        for (int i = 1; i < (int)path.size(); i++) {
            int dr = path[i].first  - path[i-1].first;
            int dc = path[i].second - path[i-1].second;
            for (int d = 0; d < 4; d++)
                if (DR[d]==dr && DC[d]==dc) {
                    cout << DIR_NAME[d];
                    if (i < (int)path.size()-1) cout << " → ";
                    break;
                }
        }
        cout << "\n";
    }
}

// ─────────────────────────────────────────────
//  Predefined mazes
// ─────────────────────────────────────────────
vector<vector<string>> MAZES = {
    // 0 — Small (from the report/slides)
    {
        "#######",
        "S    # ",
        "### * #",
        "#   * #",
        "#   *E#",
        "#######"
    },
    // 1 — Medium
    {
        "##########",
        "S        #",
        "# ######  ",
        "#      # #",
        "###### # #",
        "#      # #",
        "# ## ### #",
        "#  #    ##",
        "## # ## ##",
        "##   #  E#"
    },
    // 2 — No solution (wall blocking)
    {
        "#####",
        "S # E",
        "#####"
    },
    // 3 — Open grid
    {
        "########",
        "S       ",
        "        ",
        "        ",
        "       E",
        "########"
    }
};

const string MAZE_NAMES[] = {
    "Small (6x7)",
    "Medium (10x10)",
    "No-Solution (3x5)",
    "Open Grid (6x8)"
};

// ─────────────────────────────────────────────
//  Menu helpers
// ─────────────────────────────────────────────
int getInt(const string& prompt, int lo, int hi) {
    int v;
    while (true) {
        cout << prompt;
        if (cin >> v && v >= lo && v <= hi) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return v;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << CLR_WARN << "  Invalid input. Enter a number between "
             << lo << " and " << hi << ".\n" << CLR_RESET;
    }
}

void printTitle() {
    separator('=', 60);
    cout << CLR_TITLE
         << "           MAZE SOLVER — Algorithms Project\n"
         << "        DFS (Recursive & Iterative)  |  BFS\n"
         << "              C++ Console Frontend\n"
         << CLR_RESET;
    separator('=', 60);
}

void printComplexityTable() {
    separator('=', 60);
    cout << CLR_TITLE << "  Time & Space Complexity Analysis\n" << CLR_RESET;
    separator('-', 60);
    cout << CLR_HEAD
         << left
         << setw(22) << "  Algorithm"
         << setw(14) << "Time"
         << setw(14) << "Space"
         << "Shortest?\n"
         << CLR_RESET;
    separator('-', 60);
    auto row = [](const string& algo, const string& t,
                  const string& s, const string& sh) {
        cout << "  " << left
             << setw(20) << algo
             << setw(14) << t
             << setw(14) << s
             << sh << "\n";
    };
    row("DFS (Recursive)", "O(V+E)", "O(V) stack", "No");
    row("DFS (Iterative)", "O(V+E)", "O(V) stack", "No");
    row("BFS",             "O(V+E)", "O(V) queue", "YES ✓");
    separator('-', 60);
    cout << "  V = total maze cells   |   E = passable edges (≤ 4V)\n";
    separator('=', 60);
}

void mazeMenu(const vector<string>& maze) {
    cout << "\n  Maze preview:\n";
    printMaze(maze);
    auto [sr, sc] = findCell(maze, 'S');
    auto [er, ec] = findCell(maze, 'E');

    if (sr == -1 || er == -1) {
        cout << CLR_WARN << "  Maze is missing S or E.\n" << CLR_RESET;
        return;
    }

    separator('=', 60);
    cout << "  Select algorithm:\n"
         << "    1) DFS — Recursive\n"
         << "    2) DFS — Iterative\n"
         << "    3) BFS (Shortest Path)\n"
         << "    4) Run ALL three\n"
         << "    0) Back\n";
    separator('-', 60);
    int choice = getInt("  > ", 0, 4);

    if      (choice == 1) runAlgorithm("DFS (Recursive)", maze, sr, sc, er, ec, 0);
    else if (choice == 2) runAlgorithm("DFS (Iterative)", maze, sr, sc, er, ec, 1);
    else if (choice == 3) runAlgorithm("BFS",             maze, sr, sc, er, ec, 2);
    else if (choice == 4) {
        runAlgorithm("DFS (Recursive)", maze, sr, sc, er, ec, 0);
        runAlgorithm("DFS (Iterative)", maze, sr, sc, er, ec, 1);
        runAlgorithm("BFS",             maze, sr, sc, er, ec, 2);
    }
    separator('=', 60);
}

// ─────────────────────────────────────────────
//  Custom maze builder
// ─────────────────────────────────────────────
vector<string> buildCustomMaze() {
    int rows, cols;
    cout << "\n  Enter number of rows    (3–20): ";
    cin >> rows; rows = max(3, min(20, rows));
    cout << "  Enter number of columns (3–40): ";
    cin >> cols; cols = max(3, min(40, cols));
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<string> maze(rows, string(cols, ' '));

    // Border walls
    for (int c = 0; c < cols; c++) { maze[0][c] = '#'; maze[rows-1][c] = '#'; }
    for (int r = 0; r < rows; r++) { maze[r][0] = '#'; maze[r][cols-1] = '#'; }

    cout << "\n  Enter each row (use '#' wall, ' ' path, 'S' start, 'E' exit).\n"
         << "  Row width must be exactly " << cols << " characters.\n\n";

    for (int r = 0; r < rows; r++) {
        string line;
        cout << "  Row " << setw(2) << r << ": ";
        getline(cin, line);
        // Pad or truncate
        while ((int)line.size() < cols) line += ' ';
        line = line.substr(0, cols);
        maze[r] = line;
    }
    return maze;
}

// ─────────────────────────────────────────────
//  Main
// ─────────────────────────────────────────────
int main() {
    printTitle();

    while (true) {
        cout << "\n" << CLR_HEAD << "  MAIN MENU\n" << CLR_RESET;
        separator('-', 60);
        cout << "  1) Select a predefined maze\n"
             << "  2) Enter a custom maze\n"
             << "  3) Show Complexity Analysis\n"
             << "  0) Exit\n";
        separator('-', 60);

        int choice = getInt("  > ", 0, 3);

        if (choice == 0) {
            cout << CLR_OK << "\n  Goodbye!\n\n" << CLR_RESET;
            break;

        } else if (choice == 1) {
            cout << "\n  Choose a maze:\n";
            for (int i = 0; i < (int)MAZES.size(); i++)
                cout << "  " << (i+1) << ") " << MAZE_NAMES[i] << "\n";
            cout << "  0) Back\n";
            separator('-', 60);
            int mc = getInt("  > ", 0, (int)MAZES.size());
            if (mc > 0) mazeMenu(MAZES[mc-1]);

        } else if (choice == 2) {
            auto custom = buildCustomMaze();
            mazeMenu(custom);

        } else if (choice == 3) {
            printComplexityTable();
        }
    }

    return 0;
}
