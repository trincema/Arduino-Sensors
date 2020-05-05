void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

enum RelayMode {
  NormallyOpened,
  NormallyClosed  
};

class OneChannelRelay {
  private:
    int inPin;
    RelayMode mode;
  public:
    OneChannelRelay(int inPin, RelayMode mode) {
        this->inPin = inPin;
        this->mode = mode;
    }
    
};
