#include <iostream>
using namespace std;
/**
 * main - the entry point.
 *
 * Description: a simple game that takes two numbers from one to seven then,
 * it allows the gamer to play 5 times then outputs the scores.
 * the game alows the use of numbers one to 6.
 */

int main() {
    int x, z;
    int sum = 0;
    cout << "Input your two lucky numbers" << endl;
    for (x = 0; x < 6; x++)
    {
        int map[7][7] = {
            {0, 0, 1, 1, 0, 1, 0},
            {1, 0, 0, 1, 1, 0, 1},
            {0, 1, 0, 1, 0, 0, 0},
            {0, 0, 0, 0, 0, 1, 0},
            {0, 1, 1, 0, 0, 1, 0},
            {0, 0, 1, 0, 0, 0, 1},
            {0, 1, 0, 0, 0, 0, 1}
        };
        if (x > 4)
        {
            cout << "Game Over" << endl;
            cout << "Your Score is: " << sum << endl;
            if (sum == 5){
                cout << "Bravo!";
            }
        }
        int x, y;
        cin >> x >> y;
        cout << map[x][y] << endl;
        z = map[x][y];
        sum += z;
    }
}
