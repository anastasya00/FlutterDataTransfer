import 'dart:io';

void startServer() async {
  final server = await ServerSocket.bind(InternetAddress.anyIPv4, 5000);
  print('Сервер запущен: порт 5000');
  server.listen((client) {
    handleConnection(client);
  });
}

void handleConnection(Socket client) {
  print('Соединение с: ${client.remoteAddress.address}:${client.remotePort}');
  client.listen((data) {
    final message = String.fromCharCodes(data).trim();
    if (message.length > 2 && int.tryParse(message) != null && int.parse(message) % 32 == 0) {
      print('Получены корректные данные: ${message}');
    } else {
      print('Ошибка: получены неккоректные данные.');
    }
  });
}