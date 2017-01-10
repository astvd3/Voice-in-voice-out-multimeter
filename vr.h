#define CMD_BREAK       'b' // abort recog or ping
#define CMD_SLEEP       's' // go to power down
#define CMD_KNOB        'k' // set si knob <1>
#define CMD_LEVEL       'v' // set sd level <1>
#define CMD_LANGUAGE    'l' // set si language <1>
#define CMD_TIMEOUT     'o' // set timeout <1>
#define CMD_RECOG_SI    'i' // do si recog from ws <1>
#define CMD_TRAIN_SD    't' // train sd command at group <1> pos <2>
#define CMD_GROUP_SD    'g' // insert new command at group <1> pos <2>
#define CMD_UNGROUP_SD  'u' // remove command at group <1> pos <2>
#define CMD_RECOG_SD    'd' // do sd recog at group <1> (0 = trigger mixed si/sd)
#define CMD_ERASE_SD    'e' // reset command at group <1> pos <2>
#define CMD_NAME_SD     'n' // label command at group <1> pos <2> with length <3> name <4-n>
#define CMD_COUNT_SD    'c' // get command count for group <1>
#define CMD_DUMP_SD     'p' // read command data at group <1> pos <2>
#define CMD_MASK_SD     'm' // get active group mask
#define CMD_RESETALL    'r' // reset all commands and groups
#define CMD_ID          'x' // get version id
#define CMD_DELAY       'y' // set transmit delay <1> (log scale)
#define CMD_BAUDRATE    'a' // set baudrate <1> (bit time, 1=>115200)
#define CMD_QUERY_IO    'q' // configure, read or write I/O pin <1> of type <2>
#define CMD_PLAY_SX     'w' // wave table entry <1-2> (10-bit) playback at volume <3>
#define CMD_PLAY_DTMF   'w' // play (<1>=-1) dial tone <2> for duration <3>
#define CMD_DUMP_SX     'h' // dump wave table entries
#define CMD_DUMP_SI     'z' // dump si settings for ws <1> (or total ws count if -1)
#define CMD_SEND_SN     'j' // send sonicnet token with bits <1> index <2-3> at time <4-5>
#define CMD_RECV_SN     'f' // receive sonicnet token with bits <1> rejection <2> timeout <3-4>

#define STS_MASK        'k' // mask of active groups <1-8>
#define STS_COUNT       'c' // count of commands <1> (or number of ws <1>)
#define STS_AWAKEN      'w' // back from power down mode
#define STS_DATA        'd' // provide training <1>, conflict <2>, command label <3-35> (counted string)
#define STS_ERROR       'e' // signal error code <1-2>
#define STS_INVALID     'v' // invalid command or argument
#define STS_TIMEOUT     't' // timeout expired
#define STS_INTERR      'i' // back from aborted recognition (see 'break')
#define STS_SUCCESS     'o' // no errors status
#define STS_RESULT      'r' // recognised sd command <1> - training similar to sd <1>
#define STS_SIMILAR     's' // recognised si <1> (in mixed si/sd) - training similar to si <1>
#define STS_OUT_OF_MEM  'm' // no more available commands (see 'group')
#define STS_ID          'x' // provide version id <1>
#define STS_PIN         'p' // return pin state <1>
#define STS_TABLE_SX    'h' // table entries count <1-2> (10-bit), table name <3-35> (counted string)
#define STS_GRAMMAR     'z' // si grammar: flags <1>, word count <2>, labels... <3-35> (n counted strings)
#define STS_TOKEN       'f' // received sonicnet token <1-2>

// protocol arguments are in the range 0x40 (-1) to 0x60 (+31) inclusive
#define ARG_MIN     0x40
#define ARG_MAX     0x60
#define ARG_ZERO    0x41

#define ARG_ACK     0x20    // to read more status arguments

#define DEF_TIMEOUT 100
#define WAKE_TIMEOUT 200
#define PLAY_TIMEOUT 5000
#define NO_TIMEOUT 0

      uint8_t _command = 1;
      uint8_t _builtin = 1;
      uint8_t _error = 1;
      uint8_t _timeout = 1;
      uint8_t _invalid = 1;
      uint8_t _memfull = 1;
      uint8_t _conflict = 1;
      uint8_t _token = 1;
	  uint8_t _status = 1;
	  
	  
  uint8_t _value , idx;
  

int isTimeout() { return _timeout; }

void send(uint8_t c)
{
  _delay_ms(1);
  write(c);
}

void sendCmd(int8_t c)
{
	_delay_ms(1);
	write(c);
}

void sendArg(int8_t c)
{
  send(c + ARG_ZERO);
}

void sendGroup(int8_t c)
{
  _delay_ms(1);
  write(c + ARG_ZERO);
  _delay_ms(19); // worst case time to cache a full group in memory
}

int recv(int16_t timeout) // negative means forever
{
  while (timeout != 0)
  {
    _delay_ms(1);
    if (timeout > 0)
      --timeout;
  }
  return read();
}

int recvArg(int8_t c, int16_t timeout)
{
  send(ARG_ACK);
  int r = recv(timeout);
  c = r - ARG_ZERO;
  return r >= ARG_MIN && r <= ARG_MAX;
}

int detect()
{
  uint8_t i;
  for (i = 0; i < 5; ++i)
  {
    sendCmd(CMD_BREAK);

    if (recv(WAKE_TIMEOUT) == STS_SUCCESS)
      return 1;
  }
  return 0;
}

int stop()
{
  sendCmd(CMD_BREAK);

  uint8_t rx = recv(WAKE_TIMEOUT);
  if (rx == STS_INTERR || rx == STS_SUCCESS)
    return 1;
  return 0;
}

int setLanguage(int8_t lang)
{        
  sendCmd(CMD_LANGUAGE);
  sendArg(lang);

  if (recv(DEF_TIMEOUT) == STS_SUCCESS)
    return 1;
  return 0;
}

int setTimeout(int8_t seconds)
{
  sendCmd(CMD_TIMEOUT);
  sendArg(seconds);

  if (recv(DEF_TIMEOUT) == STS_SUCCESS)
    return 1;
  return 0;
}

int changeBaudrate(int8_t baud)
{
  sendCmd(CMD_BAUDRATE);
  sendArg(baud);

  if (recv(DEF_TIMEOUT) == STS_SUCCESS)
    return 1;
  return 0;
}

void recognizeCommand(int8_t group)
{
  sendCmd(CMD_RECOG_SD);
  sendArg(group);
}

void recognizeWord(int8_t wordset)
{
  sendCmd(CMD_RECOG_SI);
  sendArg(wordset);
}

int hasFinished()
{
  int8_t rx = recv(NO_TIMEOUT);
  if (rx < 0)
  {
    return 0;
  }
  _status = 0;
  lcd_gotoxy2(8);
  lcd_showvalue(rx);
  switch (rx)
  {
  case STS_SUCCESS:
    return 1;
  
  case STS_SIMILAR:
    _builtin = 1;
    goto GET_WORD_INDEX;

  case STS_RESULT:
    _command = 1;
  
  GET_WORD_INDEX:
   /* if (recvArg(rx, DEF_TIMEOUT))
    {
      _value = rx;
      return 1;
    }*/
	send(ARG_ACK);
	int r = recv(DEF_TIMEOUT);
	_value = r - ARG_ZERO;
    break;
    
  case STS_TOKEN:
    _token = 1;
  
    if (recvArg(rx, DEF_TIMEOUT))
    {
      _value = rx << 5;
      if (recvArg(rx, DEF_TIMEOUT))
      {
        _value |= rx;
        return 1;
      }
    }
    break;
    
  case STS_TIMEOUT:
    _timeout = 1;
    return 1;
    
  case STS_INVALID:
    _invalid = 1;
    return 1;
    
  case STS_ERROR:
    _error = 1;
    if (recvArg(rx, DEF_TIMEOUT))
    {
      _value = rx << 4;
      if (recvArg(rx, DEF_TIMEOUT))
      {
        _value |= rx;
        return 1;
      }
    }
    break;
  }

  // unexpected condition (communication error)
  _status = 0;
  _error = 1;
  return 1;
}

int8_t getCommand() { return _command ? _value : -1; }

int8_t getWord() { return _builtin ? _value : -1; }

int setPinOutput(int8_t pin, int8_t value)
{
  sendCmd(CMD_QUERY_IO);
  sendArg(pin);
  sendArg(value);

  if (recv(DEF_TIMEOUT) == STS_SUCCESS)
    return 1;
  return 0;
}

int dumpCommand(int8_t group, int8_t index, char* name, uint8_t training)
{
  sendCmd(CMD_DUMP_SD);
  sendGroup(group);
  sendArg(index);

  if (recv(DEF_TIMEOUT) != STS_DATA)
    return 0;
  
  int8_t rx;
  if (!recvArg(rx, DEF_TIMEOUT))
    return 0;
  training = rx & 0x07;
  if (rx == -1 || training == 7)
    training = 0;
  
  _status = 0;
  _conflict = (rx & 0x18) != 0;
  _command = (rx & 0x08) != 0;
  _builtin = (rx & 0x10) != 0;
  
  if (!recvArg(rx, DEF_TIMEOUT))
    return 0;
  _value = rx;

  if (!recvArg(rx, DEF_TIMEOUT))
    return 0;
  int8_t len = rx == -1 ? 32 : rx;
  for ( ; len > 0; --len, ++name)
  {
    if (!recvArg(rx, DEF_TIMEOUT))
      return 0;
    if (rx == '^' - ARG_ZERO)
    {
      if (!recvArg(rx, DEF_TIMEOUT))
        return 0;
      *name = '0' + rx;
      --len;
    }
    else
    {
      *name = ARG_ZERO + rx;
    }
  }
  *name = 0;
  return 1;
}

int playSound(int16_t index, int8_t volume)
{
  sendCmd(CMD_PLAY_SX);
  sendArg((index >> 5) & 0x1F);
  sendArg(index & 0x1F);
  sendArg(volume);

  if (recv(PLAY_TIMEOUT) == STS_SUCCESS)
    return 1;
  return 0;
}