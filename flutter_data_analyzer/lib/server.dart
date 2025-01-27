import 'dart:convert';
import 'dart:io';
import 'dart:async';

// Глобальный поток для передачи данных в приложение
final _controller = StreamController<String>.broadcast();

Stream<String> get messageStream => _controller.stream;

void startServer() async {
  final server = await ServerSocket.bind(InternetAddress.anyIPv4, 5000);
  print('Сервер запущен: порт 5000');
  
  server.listen((client) {
    handleConnection(client);
  });
}

void handleConnection(Socket client) {
  print('Соединение с: ${client.remoteAddress.address}:${client.remotePort}');
  
  client.listen(
    (data) {
      final rawMessage = String.fromCharCodes(data);
      final message = rawMessage.trim();

      if (message.length >= 2) {
        final number = int.tryParse(message);
        if (number != null && number % 32 == 0) {
          final successMessage = 'Получены данные: $message';
          print(successMessage);
          _controller.add(successMessage);
        } else {
          final errorMessage = 'Ошибка: число "$message" не кратно 32.';
          print(errorMessage);
          _controller.add(errorMessage);
        }
      } else {
        final errorMessage = 'Ошибка: число "$message" слишком короткое.';
        print(errorMessage);
        _controller.add(errorMessage);
      }
    },
    onError: (error) {
      print('Ошибка соединения: $error');
    },
    onDone: () {
      print('Клиент отключился: ${client.remoteAddress.address}:${client.remotePort}');
    },
  );
}