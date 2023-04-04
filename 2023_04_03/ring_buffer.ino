#define RING_BUFFER_SIZE 8
#define TEMP_BUFFER_SIZE 64
#define check_pin        40
static char ring_buff[RING_BUFFER_SIZE]={0,};
static int data = 0;

void reset_buffer(){
 for(int j = 0; j < RING_BUFFER_SIZE ; j++)
    {
      ring_buff[j] = '0';
    }
}

void ring_buffer(){
  char read_temp[TEMP_BUFFER_SIZE];
  if(Serial.available() >0)
  {
    reset_buffer();
    byte leng = Serial.readBytes(read_temp, TEMP_BUFFER_SIZE);
    digitalWrite(check_pin,HIGH);

    Serial.print("INPUT data Lengh : "); Serial.println(leng);
    for(int i = 0; i < leng; i++)
    {
      ring_buff[data] = read_temp[i];
      Serial.print(read_temp[i]);
      data++;
      if(data >= RING_BUFFER_SIZE) data = 0;
    }
    Serial.println(" ");        Serial.print("data no : "); Serial.println(data);
    for(int j = 0; j < RING_BUFFER_SIZE ; j++)
    {
      Serial.print(ring_buff[j]);   
    }
    Serial.println(" ");
    digitalWrite(check_pin,LOW);
  }
  
}

void setup(){
//put your setup code here, to run once:
  Serial.begin(38400); 
  pinMode (check_pin,OUTPUT);
}

void loop(){
  ring_buffer();
  
}
