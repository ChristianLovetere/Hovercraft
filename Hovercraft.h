#ifndef HOVERCRAFT_H
#define HOVERCRAFT_H

#define BUFFERSIZE 10
class circBuffer {
  public:
    int buf[BUFFERSIZE] = {1000};
    byte iter;

    circBuffer(){
      iter = 0;
    }

    void append(int num){
      buf[iter] = num;
      iterUp();
    }

    void printBuf(){
      String p;
      for(int i = 0; i < BUFFERSIZE; i++){
        p += String(buf[i]); //append an entry to the string
        if(i != BUFFERSIZE -1) //no comma after last entry
          p += ", "; //append a comma and space to the string
      }
      Serial.println(p);
    }

    int getBufSize(){
      return BUFFERSIZE;
    }

  private:
    void iterUp(){
      if (iter >= BUFFERSIZE-1) //if over buf size, go back to 0th index
        iter = 0;
      else
        iter++;
    }
}

#endif //HOVERCRAFT_H
