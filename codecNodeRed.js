// Verificamos que el dato exista para evitar errores
if (msg.payload.object && msg.payload.object.contador !== undefined) {
    
    // Extraemos solo el número 39 y lo convertimos en el mensaje principal
    msg.payload = msg.payload.object.contador;
    
    // (Opcional) Si quieres mantener el topic para saber de qué sensor viene:
    // msg.topic = "contador_heltec";
    
    return msg;
}