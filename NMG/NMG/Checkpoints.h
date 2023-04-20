#ifndef CHECKPOINTS_H
#define CHECKPOINTS_H

class Checkpoint
{
private:
    int points[2][2];
public:
    Checkpoint(int checkpoints[2][2]) {
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                points[i][j] = checkpoints[i][j];
            }
        }
     };

};
#endif