import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'server.dart';

void main() {
  runApp(MyApp());
  startServer();
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (context) => MyHomePageState(),
      child: MaterialApp(
        debugShowCheckedModeBanner: false,
        title: 'Data Receiver',
        theme: ThemeData(
          useMaterial3: true,
          colorScheme: ColorScheme.fromSeed(seedColor: Colors.blue),
        ),
        home: MyHomePage(),
      ),
    );
  }
}

class MyHomePage extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    var appState = context.watch<MyHomePageState>();

    return Scaffold(
      appBar: AppBar(
        title: Text('Данные от клиента:'),
      ),
      body: Center(
        child: Text(
          appState.receivedData,
          style: TextStyle(fontSize: 18),
          textAlign: TextAlign.center,
        ),
      ),
    );
  }
}

class MyHomePageState extends ChangeNotifier {
  String _receivedData = 'Ожидание данных...';
  String get receivedData => _receivedData;

  MyHomePageState() {
    _subscribeToMessages();
  }

  void _subscribeToMessages() {
    messageStream.listen((message) {
      _receivedData = message;
      notifyListeners();
    });
  }
}