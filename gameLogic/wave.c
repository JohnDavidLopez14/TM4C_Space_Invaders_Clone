#define MAX_WAVES 4
#define WAVEV .001
#define VWAVESPACING 14
#define MAX_WAVE_ENEMIES 5
#define MAX_ENEMIES (MAX_WAVES * MAX_WAVE_ENEMIES) // can only fit enemies across the screen max
#define ENEMYWIDTH 16
#define RAND_ENEMIES 6

typedef struct{
  const Bitmap *sprite;
  unsigned int health;
  unsigned int points;
  unsigned int active;
  unsigned int xPos;
  float xReal;
} Enemy;

typedef struct{
  unsigned int active;
  Enemy *enemyList;
  int listLength;
  int yPos;
  float yReal;
  float dy;
} Wave;

typedef struct{
  const Bitmap *sprite;
  unsigned int health;
  unsigned int points;
} RandomEnemy;

Enemy Enemies[MAX_ENEMIES];
Wave Waves[MAX_WAVES];

  // Initialize randomEnemyArray
  RandomEnemy RandomEnemies[RAND_ENEMIES] = {
    {&smallEnemy30PointA, 30, 30},
    {&smallEnemy30PointB, 30, 30},
    {&smallEnemy20PointA, 20, 20},
    {&smallEnemy20PointB, 20, 20},
    {&smallEnemy10PointA, 10, 10},
    {&smallEnemy10PointB, 10, 10}
  };

  // Initialize wave array
  for (int i = 0; i < MAX_WAVES; i++){
    Wave *wave = &Waves[i];
    wave->active = 0;
    wave->enemyList = &Enemies[i * MAX_WAVE_ENEMIES];
    wave->listLength = 0;
    wave->yPos = 0;
    wave->yReal = 0;
    wave->dy = 0;
  }

  // Searches for a Wave that is inactive
// returns the inactive wave and sets currentWaveIndex to that Wave's index
Wave *WaveSearch(Wave *waveList, unsigned int length, int *currentWaveIndex){
  for(unsigned int i = 0; i < length; i++){
    Wave *currentWave = &waveList[i];
    if(currentWave->active == 0){
      *currentWaveIndex = i;
      return currentWave;
    }
  }
  return NULL;
}

// generate a location for the enemy given the number of enemies and which enemy
unsigned int EnemySpacing(int enemyNumber, int enemyIndex){
  float totalWidth = enemyNumber * ENEMYWIDTH;
  float spacing = (SCREENW - totalWidth) / (enemyNumber + 1.0f);
  float x = spacing * (enemyIndex + 1) + ENEMYWIDTH * enemyIndex;
  return (unsigned int)(x + 0.5f);  // round to nearest int
}

// initializes the enemies
void InitializeWave(Wave *currentWave,unsigned long enemyNumber, float velocity, RandomEnemy *RandomEnemyList, unsigned int RandomEnemyLength){
  // Initialize wave
  currentWave->active = 1;
  currentWave->dy = velocity;
  currentWave->listLength = enemyNumber;
  currentWave->yPos = 0;
  currentWave->yReal = (float) currentWave->yPos;

  // initialize wave enemies
  for (int i = 0; i < currentWave->listLength; i++){
    RandomEnemy *randomEnemy = &RandomEnemyList[Random() % RandomEnemyLength];
    Enemy *currentEnemy = &currentWave->enemyList[i];
    currentEnemy->sprite = randomEnemy->sprite;
    currentEnemy->health = randomEnemy->points;
    currentEnemy->points = randomEnemy->points;
    currentEnemy->active = 1;
    currentEnemy->xPos = EnemySpacing(enemyNumber, i);
    currentEnemy->xReal = (float) currentEnemy->xPos;
  }
}

void SpawnWave(int *currentWaveIndex, RandomEnemy *RandomEnemyList, unsigned int RandomEnemyLength){
  // if the no waves are spawned, the most recent wave in active, or if the most recent wave is far enough down the screen to spawn a new wave
  if (*currentWaveIndex == -1 || Waves[*currentWaveIndex].active == 0 || Waves[*currentWaveIndex].yPos > VWAVESPACING){
    Wave *currentWave = WaveSearch(Waves, MAX_WAVES, currentWaveIndex);
    if (currentWave != NULL){ // if we have a wave available to spawn
      unsigned long enemyNumber = (Random() % 5) + 1;
      InitializeWave(currentWave, enemyNumber, WAVEV, RandomEnemyList, RandomEnemyLength);
    }
  }
}

      Wave *currentWave = &Waves[currentWaveIndex];
			snprintf(buffer,sizeof(buffer),"current wave index: %d", currentWaveIndex);
      for(int i = 0; i < currentWave->listLength; i++){
        Enemy *currentEnemy = &currentWave->enemyList[i];
        Nokia5110_PrintBMP(currentEnemy->xPos,currentWave->yPos,currentEnemy->sprite->bmp, 0);
				snprintf(buffer, sizeof(buffer),"Current Enemy health: %d, xPos: %d, yPos %d\r\n", currentEnemy->health, currentEnemy->xPos, currentWave->yPos);
      }