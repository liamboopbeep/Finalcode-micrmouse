#include <iostream>
#include <queue>
#include <stack>
#include <C:\Users\Admin\Documents\Projects\Micromouse\mms2\yes\API.cpp>
using namespace std;

/*
0 = north
1 = NE
2 = east
3 = SE
4 = south
5 = SW
6 = west
7 = NW
*/

int cur_facing = 0;    // Dir de di 8 huong
int cur_direction = 0; // Dir theo 4 huong
std::stack<int> dir_stack;
std::stack<int> act_stack;

// Toạ độ [x, y]
int cur_position[2] = {0, 0};

void log(const std::string &text)
{
    std::cerr << text << std::endl;
}

typedef struct coor
{
    int colX;
    int rowY;

} coord;

typedef struct statet
{
    coord cur;
    struct statet *parent;
    int action;
    int cur_dir;
} state;

coord start{0, 0};
/// @brief Chỉnh tùy theo mê cung
const int rows = 16;
const int cols = 16;
coord goal1{7, 7};
coord goal2{7, 8};
coord goal3{8, 7};
coord goal4{8, 8};
///////////////////////////////
std::stack<coord> loc_stack;
std::queue<state *> frontier;

typedef struct cell_infos
{
    // variables for north0,east1,south2,west3 walls
    bool walls[4];
    bool visited;
    bool dead;
} cell_info;

typedef struct wall_mazes
{
    cell_info cells[cols][rows];
} wall_maze;

wall_maze maze; // init maze

bool isValid(int x, int y)
{
    return (x >= 0 && x < cols && y >= 0 && y < rows);
} // check cell

void update_position(int &cur_direction)
{
    switch (cur_direction)
    {
    case 0: // NORTH
        cur_position[1] += 1;
        break;
    case 1: // EAST
        cur_position[0] += 1;
        break;
    case 2: // SOUTH
        cur_position[1] -= 1;
        break;
    case 3: // WEST
        cur_position[0] -= 1;
        break;
    }
}

void update_direction(int &cur_direction, int turn_direction)
{
    cur_direction = (cur_direction + turn_direction) % 4;
    if (cur_direction < 0)
    {
        cur_direction += 4;
    }
}

void get_walls()
{
    maze.cells[cur_position[0]][cur_position[1]].walls[cur_direction] = API::wallFront();
    maze.cells[cur_position[0]][cur_position[1]].walls[(cur_direction + 1) % 4] = API::wallRight();
    maze.cells[cur_position[0]][cur_position[1]].walls[(cur_direction + 2) % 4] = false;
    maze.cells[cur_position[0]][cur_position[1]].walls[(cur_direction + 3) % 4] = API::wallLeft();
    if (cur_position[0] == 0 && cur_position[1] == 0)
    {
        maze.cells[cur_position[0]][cur_position[1]].walls[2] = true;
    }
}

void mark_visited_api()
{
    API::setColor(cur_position[0], cur_position[1], 'G');
}

void mark_solution_api()
{
    API::setColor(cur_position[0], cur_position[1], 'B');
    API::setText(cur_position[0], cur_position[1], "Sol");
}

void mark_bfs_api(int x, int y)
{
    API::setColor(x, y, 'c');
    API::setText(x, y, "dfs");
}

void mark_bktrk_api(int x, int y)
{
    API::setColor(x, y, 'o');
    API::setText(x, y, "Path");
}
void move_forward()
{
    API::moveForward();
    update_position(cur_direction);
    // log("foward");
}

void turn_right()
{
    API::turnRight();
    update_direction(cur_direction, 1);
    // log("Right");
}

void turn_left()
{
    API::turnLeft();
    update_direction(cur_direction, -1);
    // log("Left");
}

void turn_around()
{
    turn_right();
    turn_right();
    // log("Around");
}

void set_dir(int _dir)
{
    if (_dir == cur_direction)
    {
        return;
    }
    if (_dir == (cur_direction + 1) % 4)
    {
        turn_right();
        return;
    }
    if (_dir == (cur_direction + 2) % 4)
    {
        turn_right();
        turn_right();
        return;
    }
    turn_left();
    return;
}

void turn_toward(coord nextStep, int &cur_direction)
{

    int _dir = cur_direction;
    if (cur_position[0] == nextStep.colX)
    {
        if (cur_position[1] - nextStep.rowY == 1)
        {
            _dir = 2;
        }
        else
        {
            _dir = 0;
        }
    }
    else
    {
        if (cur_position[0] - nextStep.colX == 1)
        {
            _dir = 3;
        }
        else
        {
            _dir = 1;
        }
    }
    set_dir(_dir);
}

void dfs_map_maze(wall_maze &maze)
{
    coord nextCell;
    coord nextStep;
    if (!maze.cells[cur_position[0]][cur_position[1]].visited)
    {
        maze.cells[cur_position[0]][cur_position[1]].visited = true;
        get_walls();
        mark_visited_api();

        if (!maze.cells[cur_position[0]][cur_position[1]].walls[0] && !maze.cells[cur_position[0]][cur_position[1] + 1].visited)
        {
            nextCell.rowY = cur_position[1] + 1;
            nextCell.colX = cur_position[0];
            if (isValid(nextCell.colX, nextCell.rowY))
            {
                loc_stack.push(nextCell);
            }
        } // upper cell

        if (!maze.cells[cur_position[0]][cur_position[1]].walls[1] && !maze.cells[cur_position[0] + 1][cur_position[1]].visited)
        {
            nextCell.rowY = cur_position[1];
            nextCell.colX = cur_position[0] + 1;
            if (isValid(nextCell.colX, nextCell.rowY))
            {
                loc_stack.push(nextCell);
            }
        } // right cell

        if (!maze.cells[cur_position[0]][cur_position[1]].walls[2] && !maze.cells[cur_position[0]][cur_position[1] - 1].visited)
        {
            nextCell.rowY = cur_position[1] - 1;
            nextCell.colX = cur_position[0];
            if (isValid(nextCell.colX, nextCell.rowY))
            {
                loc_stack.push(nextCell);
            }
        } // lower cell

        if (!maze.cells[cur_position[0]][cur_position[1]].walls[3] && !maze.cells[cur_position[0] - 1][cur_position[1]].visited)
        {
            nextCell.rowY = cur_position[1];
            nextCell.colX = cur_position[0] - 1;
            std::cerr << nextCell.colX << nextCell.rowY;
            if (isValid(nextCell.colX, nextCell.rowY))
            {
                std::cerr << isValid(nextCell.colX, nextCell.rowY) << endl;
                loc_stack.push(nextCell);
            }
        } // left cell
    }
    std::cerr << "queue size " << loc_stack.size() << endl;
    while (true)
    {
        if (loc_stack.empty())
        {
            if (!(cur_position[0] == 0 && cur_position[1] == 0))
            {
                set_dir((dir_stack.top() + 2) % 4);
                dir_stack.pop();
                move_forward();
                dfs_map_maze(maze);
            }
            return;
        }

        nextStep = loc_stack.top();
        loc_stack.pop();
        if (!maze.cells[nextStep.colX][nextStep.rowY].visited)
        {
            break;
        }
    }
    std::cerr << "Vị trí " << cur_position[0] << cur_position[1] << endl;
    std::cerr << "Ô phải tới " << nextStep.colX << nextStep.rowY << endl;
    std::cerr << "Tường " << maze.cells[cur_position[0]][cur_position[1]].walls[0] << maze.cells[cur_position[0]][cur_position[1]].walls[1] << maze.cells[cur_position[0]][cur_position[1]].walls[2] << maze.cells[cur_position[0]][cur_position[1]].walls[3] << endl;
    std::cerr << "Hướng " << cur_direction << endl;

    if ((cur_position[0] == nextStep.colX && nextStep.rowY - cur_position[1] == 1 && !maze.cells[cur_position[0]][cur_position[1]].walls[0]) ||
        (cur_position[0] == nextStep.colX && nextStep.rowY - cur_position[1] == -1 && !maze.cells[cur_position[0]][cur_position[1]].walls[2]) ||
        (cur_position[1] == nextStep.rowY && nextStep.colX - cur_position[0] == 1 && !maze.cells[cur_position[0]][cur_position[1]].walls[1]) ||
        (cur_position[1] == nextStep.rowY && nextStep.colX - cur_position[0] == -1 && !maze.cells[cur_position[0]][cur_position[1]].walls[3]))
    {
        turn_toward(nextStep, cur_direction);
        dir_stack.push(cur_direction);
        move_forward();
    }
    else
    {
        loc_stack.push(nextStep);
        std::cerr << (dir_stack.top() + 2) % 4 << endl;
        set_dir((dir_stack.top() + 2) % 4);
        dir_stack.pop();
        move_forward();
    }
    std::cerr << "-----------------" << endl;
    dfs_map_maze(maze);
}

state *find_shortest_path(wall_maze &maze, queue<state *> &frontier)
{
    coord north_loc;
    coord east_loc;
    coord south_loc;
    coord west_loc;

    coord next_state_coord;
    state *next_state;

    state *north_st;
    state *east_st;
    state *south_st;
    state *west_st;

    for (int i = 0; i < cols; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            maze.cells[i][j].visited = 0;
        }
    }
    state *first_state = new state{start, nullptr, -1, 0};
    frontier.push(first_state);
    while (!frontier.empty())
    {
        next_state = frontier.front();
        frontier.pop();
        maze.cells[next_state->cur.colX][next_state->cur.rowY].visited = true;
        mark_bfs_api(next_state->cur.colX, next_state->cur.rowY);

        next_state_coord.colX = next_state->cur.colX;
        next_state_coord.rowY = next_state->cur.rowY;

        if ((next_state->cur.colX == goal1.colX && next_state->cur.rowY == goal1.rowY) || (next_state->cur.colX == goal2.colX && next_state->cur.rowY == goal2.rowY) || (next_state->cur.colX == goal3.colX && next_state->cur.rowY == goal3.rowY) || (next_state->cur.colX == goal4.colX && next_state->cur.rowY == goal4.rowY))
        {
            return next_state;
        }

        if (!maze.cells[next_state->cur.colX][next_state->cur.rowY].walls[0])
        {
            north_loc.colX = next_state->cur.colX;
            north_loc.rowY = next_state->cur.rowY + 1;
        }

        if (!maze.cells[next_state->cur.colX][next_state->cur.rowY].walls[1])
        {
            east_loc.colX = next_state->cur.colX + 1;
            east_loc.rowY = next_state->cur.rowY;
        }

        if (!maze.cells[next_state->cur.colX][next_state->cur.rowY].walls[2])
        {
            south_loc.colX = next_state->cur.colX;
            south_loc.rowY = next_state->cur.rowY - 1;
        }

        if (!maze.cells[next_state->cur.colX][next_state->cur.rowY].walls[3])
        {
            west_loc.colX = next_state->cur.colX - 1;
            west_loc.rowY = next_state->cur.rowY;
        }

        if ((!maze.cells[next_state->cur.colX][next_state->cur.rowY].walls[0] && next_state->cur.colX == north_loc.colX && north_loc.rowY - next_state->cur.rowY == 1 && !maze.cells[north_loc.colX][north_loc.rowY].visited))
        {
            if (isValid(north_loc.colX, north_loc.rowY))
            {
                north_st = new state{north_loc, next_state, (0 - next_state->cur_dir) % 4, 0};
                frontier.push(north_st);
            }
        }

        if ((!maze.cells[next_state->cur.colX][next_state->cur.rowY].walls[1] && east_loc.colX - next_state->cur.colX == 1 && east_loc.rowY == next_state->cur.rowY && !maze.cells[east_loc.colX][east_loc.rowY].visited))
        {
            if (isValid(east_loc.colX, east_loc.rowY))
            {
                east_st = new state{east_loc, next_state, (1 - next_state->cur_dir) % 4, 1};
                frontier.push(east_st);
            }
        }

        if ((!maze.cells[next_state->cur.colX][next_state->cur.rowY].walls[2] && next_state->cur.colX == south_loc.colX && south_loc.rowY - next_state->cur.rowY == -1 && !maze.cells[south_loc.colX][south_loc.rowY].visited))
        {
            if (isValid(south_loc.colX, south_loc.rowY))
            {
                south_st = new state{south_loc, next_state, (2 - next_state->cur_dir) % 4, 2};
                frontier.push(south_st);
            }
        }

        if ((!maze.cells[next_state->cur.colX][next_state->cur.rowY].walls[3] && west_loc.colX - next_state->cur.colX == -1 && west_loc.rowY == next_state->cur.rowY && !maze.cells[west_loc.colX][west_loc.rowY].visited))
        {
            if (isValid(west_loc.colX, west_loc.rowY))
            {
                west_st = new state{west_loc, next_state, (3 - next_state->cur_dir) % 4, 3};
                frontier.push(west_st);
            }
        }
    }
}

void exec_shortest_path(state *sol)
{
    while (sol->parent != nullptr)
    {
        act_stack.push(sol->action);
        mark_bktrk_api(sol->cur.colX, sol->cur.rowY);
        state *parent = sol->parent;
        delete sol; // Free the memory of the current state
        sol = parent;
    }
    delete sol;

    std::cerr << act_stack.size() << endl;

    while (!act_stack.empty())
    {
        int act = act_stack.top();
        act_stack.pop();
        mark_solution_api();

        if (act < 0)
        {
            act += 4;
        }

        if (act == 1)
        {
            turn_right();
        }
        else if (act == 3)
        {
            turn_left();
        }
        move_forward();
    }
}

////////fin//////////

int main(int argc, char *argv[])
{
    set_dir(0);
    API::setColor(goal1.colX, goal1.rowY, 'y');
    API::setColor(goal2.colX, goal2.rowY, 'y');
    API::setColor(goal3.colX, goal3.rowY, 'y');
    API::setColor(goal4.colX, goal4.rowY, 'y');
    // move_forward();
    log("running...");
    dfs_map_maze(maze);
    state *solved = find_shortest_path(maze, frontier);
    set_dir(0);
    // state ptr = *find_shortest_path(maze).parent;
    // std::cerr << &ptr<<endl;
    // std::cerr << ptr.cur.colX<< ptr.cur.rowY <<endl;
    // std::cerr << find_shortest_path(maze).parent<<endl;
    exec_shortest_path(solved);
    log("done");
    return 0;
}
