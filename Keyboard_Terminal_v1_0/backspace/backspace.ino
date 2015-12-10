if(c==PS2_KC_BACKSPACE) {
  TV.set_cursor((charNum-1)*4,lineNum*6);
  TV.print(" ");
  TV.set_cursor((charNum-1)*4,lineNum*6);
  charNum--;
}