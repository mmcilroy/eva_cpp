import struct

class State:
  def __init__(self, state):
    self.state = state
  def get(self):
    self.state +=1
    return self.state-1

def processor( in_buff, out_buff ):
  msg = struct.unpack_from('@iiii?i', in_buff, 0)
  #print msg
  struct.pack_into( '@iiii?i', out_buff, 0, msg[0], msg[1], msg[2], msg[3], msg[4], processor.i.get() )
  return True
processor.i = State(0)
