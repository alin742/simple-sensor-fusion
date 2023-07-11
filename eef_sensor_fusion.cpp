// IF THIS SHOULD BE USED WITH ROS
// UNCOMMENT THE FOLLWING LINE
// #define ROS_IMPLEMENTATION

#include <strings.h>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#ifdef ROS_IMPLEMENTATION
  #include <ros/ros.h>
#else
  #define INPUT_FILE "sensor_data.txt"
  #define LINE_CAP 256
  ///////////////////////////////////////////// Once Done please set Done to 1 to check the output
  #define DONE 0
#endif

float pitchFromAcc(float ax, float az) {
  return std::atan2(ax, az);
}

float rollFromAcc(float ay, float az) {
  return std::atan2(ay, az);
}

/**
  This Function takes in accelerations and rotational velocities
   and calcultes the orientation of the sensor based on these data.
  The function takes in 3 arguments; the 3-dimensional accelerations
  of the sensor, the 3-dimensional rotational velocities of the sensor
  , and the 3-dimensional angle that should be calculated within this
  function. 

  REF1: https://en.wikipedia.org/wiki/Sensor_fusion
  REF2: https://stackoverflow.com/questions/68457455/calculation-of-yaw-pitch-and-roll-from-acceleration
  REF3: https://core.ac.uk/download/18293757.pdf
  REF4: https://www.youtube.com/watch?v=k5i-vE5rZR0

  NOTE: all the variables are passed in as pointers, C/C++ 
  does not have any way to pass arrays to a function or output arrays.
  This is why the output data is one of the arguments. The function
  mutates the output data.
**/
void sensor_fusion(float *acc, float *rot, float *output_angle) {
  // Accelerations
  float ax = acc[0];
  float ay = acc[1];
  float az = acc[2];

  // Rotational Velocities (euler)
  float ox = rot[0];
  float oy = rot[1];
  float oz = rot[2];

  //////////////////  CALCULATIONS  //////////////////








  //////////////////////////////////////////////////

  // output_angle[0] = [PUT HERE THE OUTPUT ANGLE OF PITCH];
  // output_angle[1] = [PUT HERE THE OUTPUT ANGLE OF ROLL];
  // output_angle[2] = [PUT HERE THE OUTPUT ANGLE OF YAW];
}


#ifdef ROS_IMPLEMENTATION
int main(int argc, char **argv) {
  // TODO: define ros node, subscriber callback and publish the
  // orientation data.
  ROS_INFO("Hello ROS");
  return 0;
}
#else
void readLine(FILE *file, char *buff) {
  bzero(buff, LINE_CAP*sizeof(char));
  char c = fgetc(file);
  int i = 0;
  while(std::isprint(c) && c != '\n' && i < LINE_CAP) {
    buff[i++] = c;
    c = fgetc(file);
  }
  buff[i] = '\0';
}

uint64_t getESPTime_us(char *line) {
  char time_buff[256];
  bzero(time_buff, 256);
  for (int i = 0; std::isprint(line[i]) && line[i] != ','; i++) {
    time_buff[i] = line[i];
  }
  return (uint64_t)atoll(time_buff);
}

uint64_t getData(char *line, float *acc, float *rot) {
  char data_buff[256];
  uint64_t time = 0;
  bzero(data_buff, 256*sizeof(char));
  int cell_index = 0, buff_index = 0;
  for(int i=0; std::isprint(line[i]); i++) {
    if (line[i] == ',') {
      if (cell_index == 0) {
        // printf("%s\n\n\n", data_buff);
        time = (uint64_t)atoll(data_buff);
      } else if ( 0 < cell_index && cell_index < 4 ) {
        acc[cell_index-1] = atol(data_buff)/100.0f;
      } else if ( 3 < cell_index ) {
        rot[cell_index-4] = atol(data_buff)*1.0f;
      }
      // printf("%ju\n\n\n", time);
      bzero(data_buff, 256*sizeof(char));
      buff_index = 0;
      cell_index++;
    } else {
      data_buff[buff_index++] = line[i];
    }
  }
  return time;
}

int main(int argc, char **argv) {
  FILE *f = fopen(INPUT_FILE, "r");
  char *line = (char *)malloc(LINE_CAP*sizeof(char));
  uint64_t curr_time;
  bzero(line, LINE_CAP*sizeof(char));
  float acc[3]; 
  float rot[3]; 
  float ori[3];
  int i = 0;
  while (!feof(f)) {
    readLine(f, line);
    if (feof(f)) break;
    curr_time = getData(line, acc, rot);
    #if DONE
    sensor_fusion(acc, rot, ori);
    printf("%ju -> Acc: %f,%f,%f\tRot: %f, %f, %f\tOri: %f, %f, %f\n",
      curr_time,
      acc[0],
      acc[1],
      acc[2],
      rot[0],
      rot[1],
      rot[2],
      ori[0],
      ori[1],
      ori[2]
    );
    #else
    printf("%ju -> Acc: %f,%f,%f\tRot: %f, %f, %f\n",
      curr_time,
      acc[0],
      acc[1],
      acc[2],
      rot[0],
      rot[1],
      rot[2]
    );
    #endif
  }
  free(line);
  fclose(f);
  return 0;
}
#endif