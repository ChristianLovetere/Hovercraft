//Header file with definitions for the circular buffer data structure.

#ifndef HOVERCRAFT_H
#define HOVERCRAFT_H

#define BUFSIZE 10 //size of the buffer. Larger buffer = more delay, less variance
#define PWM_OFF 1000 //PWM value to indicate no movement for the motors
class circBuf {
  public:
    int buf[BUFSIZE] = {PWM_OFF}; //buffer starts off indicating stationary
    byte iter;

    circBuf(){
      iter = 0;
    }

    void append(int num){ //add a new value to the buffer and overwrite the oldest
      buf[iter] = num;
      _iterUp();
    }

    void printBuf(){ //print the entire buffer to the serial terminal for debugging
      String p;
      for(int i = 0; i < BUFSIZE; i++){
        p += String(buf[i]); //append an entry to the string
        if(i != BUFSIZE -1) //no comma after last entry
          p += ", "; //append a comma and space to the string
      }
      Serial.println(p);
    }

    int getBufSize(){
      return BUFSIZE;
    }

  private:
    void _iterUp(){ //increase the iterator, set back to 0 if at final entry
      if (iter >= BUFSIZE-1)
        iter = 0;
      else
        iter++;
    }
}

#endif //HOVERCRAFT_H
