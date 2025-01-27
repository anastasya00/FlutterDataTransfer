import 'package:english_words/english_words.dart';
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
        title: 'Namer App',
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
      appBar: AppBar(title: Text('Data Analyzer'),),
      body: Center(
        child: Text(appState.receivedData),
      ),
    );
  }
}

class MyHomePageState extends ChangeNotifier {
  String _receivedData = '';

  String get receivedData => _receivedData;

  set receivedData(String newData) {
    _receivedData = newData;
    notifyListeners();
  }
}