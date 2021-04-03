#include "common.h"
#include "Image.h"
#include "Player.h"
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <stdio.h>

#define GLFW_DLL
#include <GLFW/glfw3.h>

using namespace std;

constexpr GLsizei WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 1024;
const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;

bool game_over = false; // флаг выхода из лабиринта
bool death = false; // флаг попадания в "пустоту"
bool exit_game = false; // флаг выхода из игры при смерти 
bool get_coin = false; // флаг получили ли мы монетку
bool change_character = true; // флаг для изменения персонажа
bool *next_level = new bool[4]; // 0 == UP 1 == DOWN 2 == LEFT 3 == RIGHT
bool first_trans = true; // флаг замены первого уровня
bool need_player = true;
int **stop_background = new int*[1024]; //
int coin_cnt = 0;


Image character("resources/character_left1.png");
Image character_left1("resources/character_left1.png");
Image character_left2("resources/character_left2.png");
Image character_left3("resources/character_left3.png");
Image character_down1("resources/character_down1.png");
Image character_down2("resources/character_down2.png");
Image character_down3("resources/character_down3.png");
Image character_right1("resources/character_right1.png");
Image character_right2("resources/character_right2.png");
Image character_right3("resources/character_right3.png");
Image character_up1("resources/character_up1.png");
Image character_up2("resources/character_up2.png");
Image character_up3("resources/character_up3.png");
Image floorr("resources/floorr1.png");
Image wall("resources/wall.jpg");
Image voidd("resources/voidd2.png"); 
Image img_game_over("resources/game_over.jpg");
Image gate_exit_game("resources/exit_game.png");
Image img_youre_win("resources/you_win.png");
Image coin("resources/coin1.png");
Image coin0("resources/coin0.png");
Image coin1("resources/coin1.png");
Image coin2("resources/coin2.png");
Image coin3("resources/coin3.png");
Image coin4("resources/coin4.png");
Image coin5("resources/coin5.png");
Image coin6("resources/coin6.png");
Image number0("resources/0.png");
Image number1("resources/1.png");
Image number2("resources/2.png");
Image number3("resources/3.png");
Image number4("resources/4.png");
Image number5("resources/5.png");
Image number6("resources/6.png");
Image number7("resources/7.png");
Image number8("resources/8.png");
Image number9("resources/9.png");
Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
Image level(WINDOW_WIDTH, WINDOW_HEIGHT, 4);

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
      Input.keys[key] = true;
		else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
	}
}

void processPlayerMovement(Player &player, bool &live, bool *next_level, bool &exit_game, int &timer, bool &get_coin, vector<int*> coins_vec, Point &coin_coords, bool &change_character)
{
  if ((Input.keys[GLFW_KEY_W] || Input.keys[GLFW_KEY_S] || Input.keys[GLFW_KEY_A] || Input.keys[GLFW_KEY_D]) && change_character) {
    if (timer == 12) // смена тайла игрока по счетчику
    {
      timer = 0;
      if (Input.keys[GLFW_KEY_W]) 
        character = character_up1;
      else if (Input.keys[GLFW_KEY_S])
        character = character_down1;
      else if (Input.keys[GLFW_KEY_A])
        character = character_left1;
      else if (Input.keys[GLFW_KEY_D])
        character = character_right1;
    } else if (timer == 4) 
    {
      if (Input.keys[GLFW_KEY_W]) 
        character = character_up2;
      else if (Input.keys[GLFW_KEY_S])
        character = character_down2;
      else if (Input.keys[GLFW_KEY_A])
        character = character_left2;
      else if (Input.keys[GLFW_KEY_D])
        character = character_right2;
    } else if (timer == 8) 
    {
      if (Input.keys[GLFW_KEY_W]) 
        character = character_up3;
      else if (Input.keys[GLFW_KEY_S])
        character = character_down3;
      else if (Input.keys[GLFW_KEY_A])
        character = character_left3;
      else if (Input.keys[GLFW_KEY_D])
        character = character_right3;
    }
    ++timer;
  }
  change_character = true;
  if (Input.keys[GLFW_KEY_W])
    player.ProcessInput(MovementDir::UP, stop_background, character, live, next_level, exit_game, get_coin, coin_coords, change_character);
  else if (Input.keys[GLFW_KEY_S])
    player.ProcessInput(MovementDir::DOWN, stop_background, character, live, next_level, exit_game, get_coin, coin_coords, change_character);
  if (Input.keys[GLFW_KEY_A])
    player.ProcessInput(MovementDir::LEFT, stop_background, character, live, next_level, exit_game, get_coin, coin_coords, change_character);
  else if (Input.keys[GLFW_KEY_D])
    player.ProcessInput(MovementDir::RIGHT, stop_background, character, live, next_level, exit_game, get_coin, coin_coords, change_character);
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;
  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "W, A, S, D - movement  "<< std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

// запись в файл текущей карты уровня 
void write_to_file(ofstream &file, char** cur_map) 
{
  for (int j = 0; j < 32; ++j) 
  {
    for (int i = 0; i < 32; ++i) 
      file << cur_map[i][j];
    file << '\n';
  }
  file.close();
}

// прорисовка уровня из файла
void draw_level(Image &screenBuffer, Image &level, int** stop_background, ifstream &myfile, char** cur_map) 
{
  char mode;
  int cnt;
  Pixel buffer;
  for (int j = 0; j < TILE_HEIGHT; ++j) 
  {
    for (int i = 0; i < TILE_WIDTH; ++i) 
    {
      cnt = 0;
      myfile.get(mode);
      while (mode == '\n')
        myfile.get(mode);
      cur_map[i][j] = mode;
      for (int y = j * TILE_HEIGHT; y < (j + 1) * TILE_HEIGHT; ++y)
      {
        for (int x = i * TILE_WIDTH; x < (i + 1) * TILE_WIDTH; ++x) 
        {
          if (mode == '#') // стена
          { 
            buffer = wall.GetPixel(x % wall.Width(), y % wall.Height());
            screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, buffer);
            level.PutPixel(x, WINDOW_WIDTH - 1 - y, buffer);
            stop_background[x][WINDOW_WIDTH - 1 - y] = 1;
          } else if (mode == 'e') // стена рядом с выходом
          { 
            buffer = gate_exit_game.GetPixel(x % gate_exit_game.Width(), y % gate_exit_game.Height());
            screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, buffer);
            level.PutPixel(x, WINDOW_WIDTH - 1 - y, buffer);
            stop_background[x][WINDOW_WIDTH - 1 - y] = 1;
          } else if (mode == ' ') // пустота
          { 
            Pixel check = voidd.GetPixel(x % voidd.Width(), y % voidd.Height());
            if ((check.r + check.g + check.b + check.a) > 0) 
            {
              screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
              level.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
            } else 
            {
              buffer = floorr.GetPixel(x % floorr.Width(), y % floorr.Height());
              screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, buffer);
              level.PutPixel(x, WINDOW_WIDTH - 1 - y, buffer);
            }
            stop_background[x][WINDOW_WIDTH - 1 - y] = 2;
          } else if (mode == 'G') // сокровища 
          { 
            Pixel check = coin1.GetPixel(x % coin1.Width(), y % coin1.Height());
            if ((check.r + check.g + check.b + check.a) > 0) 
            {
              screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
              level.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
            } else 
            {
              buffer = floorr.GetPixel(x % floorr.Width(), y % floorr.Height());
              screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, buffer);
              level.PutPixel(x, WINDOW_WIDTH - 1 - y, buffer);
            }
            stop_background[x][WINDOW_WIDTH - 1 - y] = 6;

          } else 
          {
            buffer = floorr.GetPixel(x % floorr.Width(), y % floorr.Height());
            screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, buffer);
            level.PutPixel(x, WINDOW_WIDTH - 1 - y, buffer);
            if (mode == '.') // пол
            {
              stop_background[x][WINDOW_WIDTH - 1 - y] = 0;
            } else if (mode == 'x') // выход из комнаты
            { 
              stop_background[x][WINDOW_WIDTH - 1 - y] = 3;
            } else if (mode == 'Q') // выход из лабиринта
            { 
              stop_background[x][WINDOW_WIDTH - 1 - y] = 4;
            } else if (mode =='@') // игрок
            { 
              if ((x % TILE_HEIGHT == 0) && ((WINDOW_WIDTH - 1 - y) % TILE_WIDTH == 0)) 
              {
                stop_background[x][WINDOW_WIDTH - 1 - y] = 5;
              } else 
              {
                stop_background[x][WINDOW_WIDTH - 1 - y] = 0;
              }
            }
          } 
          if ((i == 29) && (j == 0))
          {
            Pixel check = coin0.GetPixel(x % coin0.Width(), y % coin0.Height());
            if ((check.r + check.g + check.b + check.a) > 0) 
            {
              screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
              level.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
            }
          } else if ((i == 30) && (j == 0))
          {
            Pixel check;
            if ((coin_cnt / 10) == 0)
              check = number0.GetPixel(x % number0.Width(), y % number0.Height());
            if ((coin_cnt / 10) == 1)
              check = number1.GetPixel(x % number1.Width(), y % number1.Height());
            if ((coin_cnt / 10) == 2)
              check = number2.GetPixel(x % number2.Width(), y % number2.Height());
            if ((coin_cnt / 10) == 3)
              check = number3.GetPixel(x % number3.Width(), y % number3.Height());
            if ((coin_cnt / 10) == 4)
              check = number4.GetPixel(x % number4.Width(), y % number4.Height());
            if ((coin_cnt / 10) == 5)
              check = number5.GetPixel(x % number5.Width(), y % number5.Height());
            if ((coin_cnt / 10) == 6)
              check = number6.GetPixel(x % number6.Width(), y % number6.Height());
            if ((coin_cnt / 10) == 7)
              check = number7.GetPixel(x % number7.Width(), y % number7.Height());
            if ((coin_cnt / 10) == 8)
              check = number8.GetPixel(x % number8.Width(), y % number8.Height());
            if ((coin_cnt / 10) == 9)
              check = number9.GetPixel(x % number9.Width(), y % number9.Height());
            if ((check.r + check.g + check.b + check.a) > 0) 
            {
              screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
              level.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
            }
          } else if ((i == 31) && (j == 0))
          {
            Pixel check;
            if ((coin_cnt % 10) == 0)
              check = number0.GetPixel(x % number0.Width(), y % number0.Height());
            if ((coin_cnt % 10) == 1)
              check = number1.GetPixel(x % number1.Width(), y % number1.Height());
            if ((coin_cnt % 10) == 2)
              check = number2.GetPixel(x % number2.Width(), y % number2.Height());
            if ((coin_cnt % 10) == 3)
              check = number3.GetPixel(x % number3.Width(), y % number3.Height());
            if ((coin_cnt % 10) == 4)
              check = number4.GetPixel(x % number4.Width(), y % number4.Height());
            if ((coin_cnt % 10) == 5)
              check = number5.GetPixel(x % number5.Width(), y % number5.Height());
            if ((coin_cnt % 10) == 6)
              check = number6.GetPixel(x % number6.Width(), y % number6.Height());
            if ((coin_cnt % 10) == 7)
              check = number7.GetPixel(x % number7.Width(), y % number7.Height());
            if ((coin_cnt % 10) == 8)
              check = number8.GetPixel(x % number8.Width(), y % number8.Height());
            if ((coin_cnt % 10) == 9)
              check = number9.GetPixel(x % number9.Width(), y % number9.Height());
            if ((check.r + check.g + check.b + check.a) > 0) 
            {
              screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
              level.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
            }
          }
        }
      }
    }
  }
}

// высчитываем текущие пиксели для затухания 
void calculate_screen(Image &screenBuffer, Image &level, GLFWwindow* &window, float alpha, Player &player, bool need_player) 
{
  Pixel pixel_buffer;
  for (int y = 0; y < 1024; ++y)
    for (int x = 0; x < 1024; ++x) 
    {
      pixel_buffer = level.GetPixel(x, y);
      pixel_buffer = {static_cast<uint8_t>(pixel_buffer.r * alpha),
                      static_cast<uint8_t>(pixel_buffer.g * alpha),
                      static_cast<uint8_t>(pixel_buffer.b * alpha),
                      static_cast<uint8_t>(pixel_buffer.a * alpha)};
      screenBuffer.PutPixel(x, y, pixel_buffer);
    }
    if (need_player)
      player.Draw(screenBuffer, level, character, alpha);
    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
    glfwSwapBuffers(window);
}

// затухание
void fading(Image &screenBuffer, Image &level, GLFWwindow* &window, Player &player, bool need_player) 
{
  float alpha = 1.0;
    while (alpha > 0.0) 
    {
      alpha -= 0.05;
      calculate_screen(screenBuffer, level, window, alpha, player, need_player);
    }
}

// появление
void uprise(Image &screenBuffer, Image &level, GLFWwindow* &window, Player &player, bool need_player) 
{
  Pixel pixel_buffer;
  float alpha = 0.0;
    while (alpha < 1.0) 
    {
      calculate_screen(screenBuffer, level, window, alpha, player, need_player);
      alpha += 0.05;
    }
  for (int y = 0; y < 1024; ++y)
    for (int x = 0; x < 1024; ++x) 
    {
      pixel_buffer = level.GetPixel(x, y);
      screenBuffer.PutPixel(x, y, pixel_buffer);
    }
  if (need_player)
    player.Draw(screenBuffer, level, character, 1);
  glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
  glfwSwapBuffers(window);
}

// записываем буфферный файл
void false_buffer_maps(bool** &buffer_maps, char** cur_map, char** map, Image &screenBuffer, Image &level, int** &stop_background, string file_name, Point curlevel) 
{
  string file_name_ = "maps/level";
  float alpha = 1;
  file_name_ = file_name_ + map[curlevel.x][curlevel.y] + ".txt";
  ifstream myfile_(file_name_);
  draw_level(screenBuffer, level, stop_background, myfile_, cur_map);
  buffer_maps[curlevel.x][curlevel.y] = true;
  ofstream bufferfile;
  bufferfile.open(file_name);
  write_to_file(bufferfile, cur_map);
}

int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;
  
  for (int i = 0; i < 4; ++i) 
  {
    next_level[i] = false;
  }

  for (int i = 0; i < WINDOW_WIDTH; ++i)
    stop_background[i] = new int[WINDOW_HEIGHT];

  int timer = 0;
  float alpha = 1;
  vector<string> remove_files;

  char** cur_map = new char*[32];
  for (int i = 0; i < 32; ++i)
    cur_map[i]  = new char[32];

  char level_x, level_y;

  char** map = new char*[5];
  for(int i = 0; i < 5; ++i) 
    map[i] = new char[4];

  vector<int*> coins_vec;
  int coins_timer = 0;

  string line;
  ifstream mother_level("maps/mother_level.txt");
  for (int i = 0; i < 4; ++i)
  {
    getline(mother_level, line);
    for (int j = 0; j < 5; ++j) 
    {
      map[i][j] = line[j];
    }
  }

  bool** buffer_maps = new bool*[32];
  for (int i = 0; i < 32; ++i) 
  {
    buffer_maps[i]  = new bool[32];
    for (int j = 0; j < 32; ++j)
      buffer_maps[i][j] = false;
  }
  
  Point coin_coords;

  Point curlevel = {2, 2};

  string file_name = "maps/bufferlevel";
  string file_name_ = "maps/level";
  file_name_ = file_name_ + map[curlevel.x][curlevel.y] + ".txt";
  level_x = '0' + curlevel.x;
  level_y = '0' + curlevel.y;
  file_name = file_name + map[curlevel.x][curlevel.y] + level_x + level_y + ".txt";
  remove_files.push_back(file_name);
  ifstream myfile_(file_name_);

  draw_level(screenBuffer, level, stop_background, myfile_, cur_map);


  ofstream bufferfile;
  bufferfile.open(file_name);
  write_to_file(bufferfile, cur_map);
  buffer_maps[curlevel.x][curlevel.y] = true;

  ifstream myfile(file_name);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 base project", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);

	if(initGL() != 0) 
		return -1;
	
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	Point starting_pos{.x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT / 2};
	Player player;
  if (first_trans)
    for (int i = 0; i < WINDOW_WIDTH; ++i)
      for (int j = 0; j < WINDOW_HEIGHT; ++j) 
        if (stop_background[i][j] == 5) 
        {
          player.PutCoords(i, j);
          break;
        }



  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;
  //game loop
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
    glfwPollEvents();
    Point coords = player.GetCoords();

    

    processPlayerMovement(player, death, next_level, exit_game, timer, get_coin, coins_vec, coin_coords, change_character);
    if (next_level[0]) // up
    { 
      fading(screenBuffer, level, window, player, need_player);
      curlevel.x -= 1;
      file_name = "maps/bufferlevel";
      level_x = '0' + curlevel.x;
      level_y = '0' + curlevel.y;
      file_name = file_name + map[curlevel.x][curlevel.y] + level_x + level_y + ".txt";
      if (!buffer_maps[curlevel.x][curlevel.y]) {
        false_buffer_maps(buffer_maps, cur_map, map, screenBuffer, level, stop_background, file_name, curlevel);
        remove_files.push_back(file_name);
      }
      alpha = 1;
      ifstream myfile(file_name);
      draw_level(screenBuffer, level, stop_background, myfile, cur_map);
      player.PutCoords(WINDOW_WIDTH / 2, player.GetSpeed());
      player.Draw(screenBuffer, level, character, alpha);
      uprise(screenBuffer, level, window, player, need_player);
      next_level[0] = false;

    } else if (next_level[1]) // down
    { 

      fading(screenBuffer, level, window, player, need_player);
      curlevel.x += 1;
      file_name = "maps/bufferlevel";
      level_x = '0' + curlevel.x;
      level_y = '0' + curlevel.y;
      file_name = file_name + map[curlevel.x][curlevel.y] + level_x + level_y + ".txt";
      if (!buffer_maps[curlevel.x][curlevel.y]) {
        false_buffer_maps(buffer_maps, cur_map, map, screenBuffer, level, stop_background, file_name, curlevel);
        remove_files.push_back(file_name);
      }
      ifstream myfile(file_name);
      alpha = 1;
      draw_level(screenBuffer, level, stop_background, myfile, cur_map);
      player.PutCoords(WINDOW_WIDTH / 2, WINDOW_HEIGHT - player.GetSpeed() - character.Height());
      player.Draw(screenBuffer, level, character, alpha);
      uprise(screenBuffer, level, window, player, need_player);
      next_level[1] = false;

    } else if (next_level[2]) // left
    { 

      fading(screenBuffer, level, window, player, need_player);
      curlevel.y -= 1;
      file_name = "maps/bufferlevel";
      level_x = '0' + curlevel.x;
      level_y = '0' + curlevel.y;
      file_name = file_name + map[curlevel.x][curlevel.y] + level_x + level_y + ".txt";
      if (!buffer_maps[curlevel.x][curlevel.y]) {
        false_buffer_maps(buffer_maps, cur_map, map, screenBuffer, level, stop_background, file_name, curlevel);
        remove_files.push_back(file_name);
      }
      ifstream myfile(file_name);
      alpha = 1;
      draw_level(screenBuffer, level, stop_background, myfile, cur_map);
      player.PutCoords(WINDOW_WIDTH - player.GetSpeed() - character.Width(), WINDOW_HEIGHT / 2);
      player.Draw(screenBuffer, level, character, alpha);
      uprise(screenBuffer, level, window, player, need_player);
      next_level[2] = false;

    } else if (next_level[3]) // right
    { 

      fading(screenBuffer, level, window, player, need_player);
      curlevel.y += 1;
      file_name = "maps/bufferlevel";
      level_x = '0' + curlevel.x;
      level_y = '0' + curlevel.y;
      file_name = file_name + map[curlevel.x][curlevel.y] + level_x + level_y + ".txt";
      if (!buffer_maps[curlevel.x][curlevel.y]) {
        false_buffer_maps(buffer_maps, cur_map, map, screenBuffer, level, stop_background, file_name, curlevel);
        remove_files.push_back(file_name);
      }
      ifstream myfile(file_name);
      alpha = 1;
      draw_level(screenBuffer, level, stop_background, myfile, cur_map);
      player.PutCoords(player.GetSpeed(), WINDOW_HEIGHT / 2);
      player.Draw(screenBuffer, level, character, alpha);
      uprise(screenBuffer, level, window, player, need_player);
      next_level[3] = false;

    } else if (death) 
    {
      fading(screenBuffer, level, window, player, true);
      for (int y = 0; y < 1024; ++y)
        for (int x = 0; x < 1024; ++x)
          screenBuffer.PutPixel(x, 1023 - y, img_game_over.GetPixel(x % img_game_over.Width(), y % img_game_over.Height()));
      level = screenBuffer;
      uprise(screenBuffer, level, window, player, false);
      game_over = true;      
    } else if (exit_game) 
    {
      fading(screenBuffer, level, window, player, true);
      for (int y = 0; y < 1024; ++y)
        for (int x = 0; x < 1024; ++x)
          screenBuffer.PutPixel(x, 1023 - y, img_youre_win.GetPixel(x % img_youre_win.Width(), y % img_youre_win.Height()));     
      level = screenBuffer;
      uprise(screenBuffer, level, window, player, false); 
    } else if (get_coin) 
    {

        ++coin_cnt;
        cur_map[coin_coords.x / 32][31 - (coin_coords.y / 32)] = '.';
        ofstream bufferfile;
        bufferfile.open(file_name);
        write_to_file(bufferfile, cur_map);
        ifstream myfile(file_name);
        draw_level(screenBuffer, level, stop_background, myfile, cur_map);
        get_coin = false;

    } else 
    {
      player.Draw(screenBuffer, level, character, 1);
    }
    
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
    
    if (!game_over && !exit_game)
    {
      if (coins_timer == 40)
        coins_timer = 0;
      if (coins_timer < 8) 
        coin = coin1;
      else if (coins_timer < 16)
        coin = coin2;
      else if (coins_timer < 24)
        coin = coin3;
      else if (coins_timer < 32)
        coin = coin4;
      else if (coins_timer < 40)
        coin = coin5;
      ++coins_timer;
      if ((coins_timer == 8) || (coins_timer == 16) || (coins_timer == 24) || (coins_timer == 32) || (coins_timer == 40))
        for (int y = 0; y < WINDOW_HEIGHT; ++y)
          for (int x = 0; x < WINDOW_WIDTH; ++x) 
          {
            if (stop_background[x][WINDOW_WIDTH - 1 - y] == 6) 
            {
              Pixel check = coin.GetPixel(x % coin.Width(), y % coin.Height());
              if ((check.r + check.g + check.b + check.a) > 0) 
              {
                screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
                level.PutPixel(x, WINDOW_WIDTH - 1 - y, check);
              } else {
                screenBuffer.PutPixel(x, WINDOW_WIDTH - 1 - y, floorr.GetPixel(x % floorr.Width(), y % floorr.Height()));
                level.PutPixel(x, WINDOW_WIDTH - 1 - y, floorr.GetPixel(x % floorr.Width(), y % floorr.Height()));
              }
            }
          }
    }

    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

		glfwSwapBuffers(window);

    if (game_over || exit_game) 
    {
      break;
    }
	}



  while(!glfwWindowShouldClose(window)) 
  {

    glfwPollEvents();
  };

	glfwTerminate();
  const char *cstr;
  for (int i = 0; i < remove_files.size(); ++i) {
    cstr = remove_files[i].c_str();
    remove(cstr);
  }

	return 0;
}
