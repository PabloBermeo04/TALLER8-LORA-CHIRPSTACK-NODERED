// Decodificador para Payload: "AGQ=" -> 0x00 0x64 -> 100
function decodeUplink(input) {
  var bytes = input.bytes;
  var data = {};

  // Verificamos que al menos vengan 2 bytes para evitar errores
  if (bytes.length >= 2) {
      // Unimos el Byte 0 (High) y el Byte 1 (Low)
      // (bytes[0] << 8) mueve el primer byte 8 posiciones a la izquierda
      // | bytes[1] suma el segundo byte
      var valorLeido = (bytes[0] << 8) | bytes[1];
      
      data.contador = valorLeido;
  }

  return {
    data: data
  };
}