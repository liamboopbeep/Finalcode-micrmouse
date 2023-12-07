TaskHandle_t Task1;
TaskHandle_t Task2;

#include <queue>
#include <stack>

#define EncoderLA 33
#define EncoderLB 32
#define EncoderRA 35
#define EncoderRB 34

#define MotorEL 14
#define MotorLA 12
#define MotorLB 19
#define MotorER 25
#define MotorRA 26
#define MotorRB 27

#define senR 17
#define senR45 16
#define senL 21
#define senL45 22
#define senFL 23
#define senRL 4


//algorithm
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

int cur_position[2] = {0, 0};

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
    maze.cells[cur_position[0]][cur_position[1]].walls[cur_direction] = wallFront();
    maze.cells[cur_position[0]][cur_position[1]].walls[(cur_direction + 1) % 4] = wallRight();
    maze.cells[cur_position[0]][cur_position[1]].walls[(cur_direction + 2) % 4] = false;
    maze.cells[cur_position[0]][cur_position[1]].walls[(cur_direction + 3) % 4] = wallLeft();
    if (cur_position[0] == 0 && cur_position[1] == 0)
    {
        maze.cells[cur_position[0]][cur_position[1]].walls[2] = true;
    }
}

void move_forward()
{
    
    update_position(cur_direction);
    // log("foward");
}

void turn_right()
{
    
    update_direction(cur_direction, 1);
    // log("Right");
}

void turn_left()
{
    
    update_direction(cur_direction, -1);
    // log("Left");
}

void turn_around()
{
    turn_right();
    turn_right();
    // log("Around");
}

bool wallFront(){
  if(analogRead(senFL)){
    return true;
  }
  else{
    return false;
  }
}
bool wallRight(){
  if(analogRead(senL)){
    return true;
  }
  else{
    return false;
  }
}
bool wallLeft(){
  if(analogRead(senR)){
    return true;
  }
  else{
    return false;
  }
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
            //std::cerr << nextCell.colX << nextCell.rowY;
            if (isValid(nextCell.colX, nextCell.rowY))
            {
                //std::cerr << isValid(nextCell.colX, nextCell.rowY) << endl;
                loc_stack.push(nextCell);
            }
        } // left cell
    }
    //std::cerr << "queue size " << loc_stack.size() << endl;
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
    //std::cerr << "Vị trí " << cur_position[0] << cur_position[1] << endl;
    //std::cerr << "Ô phải tới " << nextStep.colX << nextStep.rowY << endl;
    //std::cerr << "Tường " << maze.cells[cur_position[0]][cur_position[1]].walls[0] << maze.cells[cur_position[0]][cur_position[1]].walls[1] << maze.cells[cur_position[0]][cur_position[1]].walls[2] << maze.cells[cur_position[0]][cur_position[1]].walls[3] << endl;
    //std::cerr << "Hướng " << cur_direction << endl;

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
        //std::cerr << (dir_stack.top() + 2) % 4 << endl;
        set_dir((dir_stack.top() + 2) % 4);
        dir_stack.pop();
        move_forward();
    }
    //std::cerr << "-----------------" << endl;
    dfs_map_maze(maze);
}

/*
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

*/


////////////////////
int _1msclock = 0;

int L_set_rpm = 1000;
float L_dutyCycle = 100;
unsigned long L_prev_time = 0;
float L_rpm = 0;
float L_rpm_error_pre = 0;
int L_deltaT;
unsigned long L_distance_covered = 0;//mm

int R_set_rpm = 2000;
float R_dutyCycle = 100;
unsigned long R_prev_time = 0;
float R_rpm = 0;
int R_deltaT;
unsigned long R_distance_covered = 0;//mm
//rpm meter
void IRAM_ATTR L_encoder_isr() {
  L_deltaT = micros()-L_prev_time;
  L_prev_time = micros();
}
void IRAM_ATTR R_encoder_isr() {
  R_deltaT = micros()-R_prev_time;
  R_prev_time = micros();
}

// setting PWM properties
const int freq = 15625;
const int L_motor_channel = 0;
const int R_motor_channel = 1;
const int resolution = 12;

void setup() {
  Serial.begin(115200); 

  pinMode(EncoderLA, INPUT_PULLUP);
  pinMode(EncoderRA, INPUT_PULLUP);

  // Attaching the ISR to encoder A
  attachInterrupt(digitalPinToInterrupt(EncoderLA), L_encoder_isr, RISING);
  attachInterrupt(digitalPinToInterrupt(EncoderRA), R_encoder_isr, RISING);


  ledcSetup(L_motor_channel, freq, resolution);
  ledcAttachPin(MotorEL, L_motor_channel);
  ledcSetup(R_motor_channel, freq, resolution);
  ledcAttachPin(MotorER, R_motor_channel);
  
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    0,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(50); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(50); 
}

void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    if(millis() - _1msclock >=1){
    L_rpm = (float)(60000000)/(L_deltaT*334);
    float L_rpm_error = L_set_rpm - L_rpm;
    L_dutyCycle += L_rpm_error * 0.005 + ((L_rpm_error-L_rpm_error_pre)/0.001) * 5;
    L_dutyCycle = constrain(L_dutyCycle,0,4095);
    ledcWrite(L_motor_channel, L_dutyCycle);
    L_rpm_error_pre = L_rpm_error;


    R_rpm = (float)(60000000)/(R_deltaT*334);
    float R_rpm_error = R_set_rpm - R_rpm;
    R_dutyCycle += R_rpm_error * 0.01;
    R_dutyCycle = constrain(R_dutyCycle,0,4095);
    ledcWrite(R_motor_channel, R_dutyCycle);
    }

  }
}

void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    set_dir(0);
    dfs_map_maze(maze);
  }
}

void loop() {
  //do not write anything
  vTaskDelete(NULL);
}
