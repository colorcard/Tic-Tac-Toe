#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
    // Windows 平台特定函数定义
    #define sleep(x) Sleep((x) * 1000)

    // 设置控制台为 UTF-8 编码
    void setConsoleUTF8() {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    }
#else
#include <unistd.h>
// 空函数，Unix 平台不需要特别设置
void setConsoleUTF8() {}
#endif
// 其余代码保持不变

// ANSI颜色代码
#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

// 背景颜色
#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"

// 全局变量
char board[3][3];  // 3×3棋盘
char currentPlayer; // 当前玩家
int gameMode;      // 游戏模式：1-人机对战，2-双人对战

// 数字键盘映射到棋盘位置
const int keypadMapping[10][2] = {
        {0, 0},  // 占位，不使用
        {2, 0},  // 1 -> 左下
        {2, 1},  // 2 -> 下中
        {2, 2},  // 3 -> 右下
        {1, 0},  // 4 -> 中左
        {1, 1},  // 5 -> 中心
        {1, 2},  // 6 -> 中右
        {0, 0},  // 7 -> 左上
        {0, 1},  // 8 -> 上中
        {0, 2}   // 9 -> 右上
};

// 函数声明
void clearScreen();
void showSplashScreen();
void showGameMenu();
void initializeBoard();
void displayBoard();
void displayGameInfo();
void playerMove(); // 新增：玩家落子函数
int checkWinSimulation();
int makeMove(int row, int column);
int checkWin();
int isBoardFull();
void changePlayer();
void computerMove();
int findWinningMove(char player);
int findBlockingMove();
void showWinAnimation(int winType, int line);
void showEndGame(int gameStatus);
int playAgain();

int main() {
    setConsoleUTF8();
    int gameStatus = 0;  // 0:继续游戏, 1:有玩家获胜, 2:平局
    int playAgainChoice = 1;

    // 随机数初始化
    srand(time(NULL));

    showSplashScreen();

    while (playAgainChoice) {
        clearScreen();
        showGameMenu();
        scanf("%d", &gameMode);

        while (gameMode != 1 && gameMode != 2) {
            printf(RED "无效选择，请重新输入(1或2): " RESET);
            scanf("%d", &gameMode);
        }

        initializeBoard();
        currentPlayer = 'X';  // X先行
        gameStatus = 0;

        while (gameStatus == 0) {
            clearScreen();
            displayBoard();
            displayGameInfo();

            if (currentPlayer == 'X' || gameMode == 2) {
                // 玩家回合，使用新的落子函数
                playerMove();

                // 检查游戏状态
                if (checkWin()) {
                    gameStatus = 1; // 胜利
                } else if (isBoardFull()) {
                    gameStatus = 2; // 平局
                } else {
                    changePlayer(); // 切换玩家
                }
            } else {
                // 电脑回合
                printf(CYAN "\n电脑正在思考...\n" RESET);
                sleep(1); // 模拟思考时间
                computerMove();

                if (checkWin()) {
                    gameStatus = 1; // 胜利
                } else if (isBoardFull()) {
                    gameStatus = 2; // 平局
                } else {
                    changePlayer(); // 切换玩家
                }
            }
        }

        // 显示最终结果
        clearScreen();
        displayBoard();
        showEndGame(gameStatus);

        // 询问是否再玩一局
        playAgainChoice = playAgain();
    }

    clearScreen();
    printf(BOLD YELLOW "\n感谢您游玩井字棋！再见！\n\n" RESET);

    return 0;
}

// 玩家落子函数
void playerMove() {
    int position;
    int valid = 0;

    while (!valid) {
        printf(BOLD "玩家 %c 请输入落子位置 (1-9): " RESET, currentPlayer);
        scanf("%d", &position);

        if (position >= 1 && position <= 9) {
            int row = keypadMapping[position][0];
            int col = keypadMapping[position][1];

            if (board[row][col] == ' ') {
                board[row][col] = currentPlayer;
                valid = 1;
            } else {
                printf(RED "\n该位置已被占用，请重新选择！\n" RESET);
                sleep(1);
                clearScreen();
                displayBoard();
                displayGameInfo();
            }
        } else {
            printf(RED "\n无效的输入，请输入1-9之间的数字！\n" RESET);
            sleep(1);
            clearScreen();
            displayBoard();
            displayGameInfo();
        }
    }
}

// 清屏函数
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 显示游戏启动画面
void showSplashScreen() {
    clearScreen();
    printf(BOLD CYAN);
    printf("\n");
    printf("  ████████╗██╗ ██████╗    ████████╗ █████╗  ██████╗    ████████╗ ██████╗ ███████╗\n");
    printf("  ╚══██╔══╝██║██╔════╝    ╚══██╔══╝██╔══██╗██╔════╝    ╚══██╔══╝██╔═══██╗██╔════╝\n");
    printf("     ██║   ██║██║            ██║   ███████║██║            ██║   ██║   ██║█████╗  \n");
    printf("     ██║   ██║██║            ██║   ██╔══██║██║            ██║   ██║   ██║██╔══╝  \n");
    printf("     ██║   ██║╚██████╗       ██║   ██║  ██║╚██████╗       ██║   ╚██████╔╝███████╗\n");
    printf("     ╚═╝   ╚═╝ ╚═════╝       ╚═╝   ╚═╝  ╚═╝ ╚═════╝       ╚═╝    ╚═════╝ ╚══════╝\n");
    printf(RESET);

    printf(YELLOW "\n\n                             极致井字棋体验" RESET);
    printf(GREEN "\n\n                         按任意键开始精彩对决..." RESET);
    getchar(); // 等待用户按键
}

// 显示游戏菜单
void showGameMenu() {
    printf(BOLD BLUE "\n\n");
    printf("              ┌───────────────────────────┐\n");
    printf("              │                           │\n");
    printf("              │       游戏模式选择        │\n");
    printf("              │                           │\n");
    printf("              │    1. 人机对战 (对战AI)   │\n");
    printf("              │    2. 双人对战            │\n");
    printf("              │                           │\n");
    printf("              └───────────────────────────┘\n\n");
    printf(RESET);

    printf("请选择游戏模式 (1-2): ");
}

// 初始化棋盘
void initializeBoard() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
}

// 显示棋盘（修改版，显示数字键盘映射）
void displayBoard() {
    printf(BOLD YELLOW "\n      井字棋游戏\n\n" RESET);

//    // 添加数字键盘映射指引
//    printf(CYAN "数字键映射:\n");
//    printf("7 | 8 | 9\n");
//    printf("─────────\n");
//    printf("4 | 5 | 6\n");
//    printf("─────────\n");
//    printf("1 | 2 | 3\n" RESET);
//    printf("\n");

    printf("  ┌───┬───┬───┐\n");

    for (int i = 0; i < 3; i++) {
        printf("  │");
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 'X') {
                printf(BOLD RED " X " RESET);
            } else if (board[i][j] == 'O') {
                printf(BOLD BLUE " O " RESET);
            } else {
                // 显示空位置对应的数字提示
                int num = 0;
                for (int k = 1; k <= 9; k++) {
                    if (keypadMapping[k][0] == i && keypadMapping[k][1] == j) {
                        num = k;
                        break;
                    }
                }
                printf(CYAN " %d " RESET, num);
            }
            printf("│");
        }
        printf("\n");

        if (i < 2) {
            printf("  ├───┼───┼───┤\n");
        }
    }

    printf("  └───┴───┴───┘\n\n");
}

// 显示游戏信息
void displayGameInfo() {
    if (gameMode == 1) {
        if (currentPlayer == 'X') {
            printf(BOLD GREEN "● 当前回合: " RED "玩家(X)\n" RESET);
            printf(BOLD GREEN "● 游戏模式: 人机对战\n" RESET);
        } else {
            printf(BOLD GREEN "● 当前回合: " BLUE "电脑(O)\n" RESET);
            printf(BOLD GREEN "● 游戏模式: 人机对战\n" RESET);
        }
    } else {
        printf(BOLD GREEN "● 当前回合: %s玩家(%c)\n" RESET,
               currentPlayer == 'X' ? RED : BLUE,
               currentPlayer);
        printf(BOLD GREEN "● 游戏模式: 双人对战\n" RESET);
    }
    printf("\n");
}

// 尝试在指定位置落子，成功返回1，失败返回0
int makeMove(int row, int column) {
    if (board[row][column] == ' ') {
        board[row][column] = currentPlayer;
        return 1;
    }
    return 0;
}

// 检查是否有玩家获胜，返回胜利类型
int checkWin() {
    // 检查行
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != ' ' &&
            board[i][0] == board[i][1] &&
            board[i][1] == board[i][2]) {
            showWinAnimation(1, i);
            return 1;
        }
    }

    // 检查列
    for (int i = 0; i < 3; i++) {
        if (board[0][i] != ' ' &&
            board[0][i] == board[1][i] &&
            board[1][i] == board[2][i]) {
            showWinAnimation(2, i);
            return 1;
        }
    }

    // 检查对角线 (左上到右下)
    if (board[0][0] != ' ' &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2]) {
        showWinAnimation(3, 0);
        return 1;
    }

    // 检查对角线 (右上到左下)
    if (board[0][2] != ' ' &&
        board[0][2] == board[1][1] &&
        board[1][1] == board[2][0]) {
        showWinAnimation(4, 0);
        return 1;
    }

    return 0;
}

// 显示获胜动画
void showWinAnimation(int winType, int line) {
    // 在这里可以实现一些简单的动画效果，如闪烁等
    clearScreen();
    displayBoard();

    printf(BOLD YELLOW "获胜线路：");

    switch(winType) {
        case 1:
            printf("第 %d 行\n\n", line);
            break;
        case 2:
            printf("第 %d 列\n\n", line);
            break;
        case 3:
            printf("主对角线\n\n");
            break;
        case 4:
            printf("副对角线\n\n", line);
            break;
    }

    sleep(1);
}

// 检查棋盘是否已满
int isBoardFull() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                return 0;
            }
        }
    }
    return 1;
}

// 切换当前玩家
void changePlayer() {
    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
}

// 电脑决策落子
void computerMove() {
    // 策略1：检查电脑是否能一步获胜
    if (findWinningMove('O')) {
        return;
    }

    // 策略2：阻止玩家一步获胜
    if (findBlockingMove()) {
        return;
    }

    // 策略3：如果中心位置空闲，选择中心
    if (board[1][1] == ' ') {
        board[1][1] = currentPlayer;
        return;
    }

    // 策略4：优先选择角落
    int corners[4][2] = {{0, 0}, {0, 2}, {2, 0}, {2, 2}};
    int availableCorners = 0;

    for (int i = 0; i < 4; i++) {
        int row = corners[i][0];
        int col = corners[i][1];
        if (board[row][col] == ' ') {
            availableCorners++;
        }
    }

    if (availableCorners > 0) {
        int cornerChoice = rand() % availableCorners;
        int current = 0;

        for (int i = 0; i < 4; i++) {
            int row = corners[i][0];
            int col = corners[i][1];
            if (board[row][col] == ' ') {
                if (current == cornerChoice) {
                    board[row][col] = currentPlayer;
                    return;
                }
                current++;
            }
        }
    }

    // 策略5：随机选择任意空位置
    int emptyCells = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                emptyCells++;
            }
        }
    }

    if (emptyCells > 0) {
        int cellChoice = rand() % emptyCells;
        int current = 0;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == ' ') {
                    if (current == cellChoice) {
                        board[i][j] = currentPlayer;
                        return;
                    }
                    current++;
                }
            }
        }
    }
}

// 查找一步获胜的位置
int findWinningMove(char player) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                // 尝试在此位置落子
                board[i][j] = player;
                // 检查是否获胜，使用不显示动画的版本
                if (checkWinSimulation()) {
                    // 如果是当前玩家，保留这步棋
                    if (player == currentPlayer) {
                        return 1;
                    }
                    // 恢复棋盘
                    board[i][j] = ' ';
                    return 0;
                }
                // 恢复棋盘
                board[i][j] = ' ';
            }
        }
    }
    return 0;
}

// 查找需要阻止对手获胜的位置
int findBlockingMove() {
    char opponent = (currentPlayer == 'X') ? 'O' : 'X';

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                // 假设对手在此位置落子
                board[i][j] = opponent;
                // 检查对手是否会获胜，使用不显示动画的版本
                if (checkWinSimulation()) {
                    // 在此位置落子阻止对手
                    board[i][j] = currentPlayer;
                    return 1;
                }
                // 恢复棋盘
                board[i][j] = ' ';
            }
        }
    }
    return 0;
}

// 显示游戏结束信息
void showEndGame(int gameStatus) {
    if (gameStatus == 1) {
        if (currentPlayer == 'X') {
            printf(BOLD RED "\n恭喜！玩家 X 获胜！\n" RESET);
            printf(YELLOW "┌───────────────────────────┐\n");
            printf("│                           │\n");
            printf("│        🎮 胜利 🎮        │\n");
            printf("│                           │\n");
            printf("└───────────────────────────┘\n" RESET);
        } else if (gameMode == 1) {
            printf(BOLD BLUE "\n很遗憾，电脑获胜了！\n" RESET);
            printf(YELLOW "┌───────────────────────────┐\n");
            printf("│                           │\n");
            printf("│        💻 AI胜利 💻       │\n");
            printf("│                           │\n");
            printf("└───────────────────────────┘\n" RESET);
        } else {
            printf(BOLD BLUE "\n恭喜！玩家 O 获胜！\n" RESET);
            printf(YELLOW "┌───────────────────────────┐\n");
            printf("│                           │\n");
            printf("│        🎮 胜利 🎮        │\n");
            printf("│                           │\n");
            printf("└───────────────────────────┘\n" RESET);
        }
    } else {
        printf(BOLD GREEN "\n游戏平局！棋逢对手！\n" RESET);
        printf(YELLOW "┌───────────────────────────┐\n");
        printf("│                           │\n");
        printf("│        🤝 平局 🤝        │\n");
        printf("│                           │\n");
        printf("└───────────────────────────┘\n" RESET);
    }
}

// 询问是否再玩一局
int playAgain() {
    char choice;
    printf(BOLD CYAN "\n是否再来一局？(y/n): " RESET);
    scanf(" %c", &choice);  // 注意前面的空格，用于跳过可能的换行符
    return (choice == 'y' || choice == 'Y');
}

// 添加一个新的函数，专用于模拟检查，不显示动画
int checkWinSimulation() {
    // 检查行
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != ' ' &&
            board[i][0] == board[i][1] &&
            board[i][1] == board[i][2]) {
            return 1;
        }
    }

    // 检查列
    for (int i = 0; i < 3; i++) {
        if (board[0][i] != ' ' &&
            board[0][i] == board[1][i] &&
            board[1][i] == board[2][i]) {
            return 1;
        }
    }

    // 检查对角线 (左上到右下)
    if (board[0][0] != ' ' &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2]) {
        return 1;
    }

    // 检查对角线 (右上到左下)
    if (board[0][2] != ' ' &&
        board[0][2] == board[1][1] &&
        board[1][1] == board[2][0]) {
        return 1;
    }

    return 0;
}

