import 'package:flutter/material.dart';
import 'package:requests/requests.dart';
import 'chart.dart';
import 'package:charts_flutter/flutter.dart' as charts;
import 'dart:math';
import 'package:intl/intl.dart';

void main() {
  runApp(MyApp());
}

String date_time() {
  var now = new DateTime.now();
  var formatter = new DateFormat('yyyy-MM-dd');
  String formattedDate = formatter.format(now);
  print(formattedDate); // 2016-01-25
  return formattedDate;
}

class MyApp extends StatefulWidget {
  MyApp({Key key}) : super(key: key);
  Response request_data;
  String activity_time = "";
  String url = 'https://reqres.in/api/users/10';
  List<AppDownloads> data;
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  void initState() {
    super.initState();

    widget.data = [
      AppDownloads(
        year: '2017',
        count: 178,
        barColor: charts.ColorUtil.fromDartColor(Colors.red),
      ),
      AppDownloads(
        year: '2018',
        count: 205,
        barColor: charts.ColorUtil.fromDartColor(Colors.blue),
      ),
      AppDownloads(
        year: '2019',
        count: 258,
        barColor: charts.ColorUtil.fromDartColor(Colors.green),
      ),
    ];
  }

  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      color: Colors.blueGrey,
      home: Scaffold(
        appBar: AppBar(
          backgroundColor: Colors.lightBlue[700],
          title: Text("Activity Tracker"),
          titleTextStyle: TextStyle(color: Colors.blueGrey),
          centerTitle: true,
        ),
        body: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Image.asset(
              'Asset/cube.png',
              height: 200,
              width: 200,
            ),
            Padding(
              padding: const EdgeInsets.all(10),
              child: Card(
                color: Colors.amberAccent[200],
                shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(30)),
                elevation: 8,
                child: InkWell(
                  onTap: () async {
                    widget.request_data = await Requests.get(
                        'https://activitytracker.mak3r.space/get_activities/Mathew/aggregate_time');

                    print(widget.request_data.json()['study']);
                    widget.data = [
                      AppDownloads(
                        year: 'exercise',
                        count: widget.request_data.json()['exercise'],
                        barColor: charts.ColorUtil.fromDartColor(Colors.red),
                      ),
                      AppDownloads(
                        year: 'read',
                        count: widget.request_data.json()['read'],
                        barColor: charts.ColorUtil.fromDartColor(Colors.blue),
                      ),
                      AppDownloads(
                        year: 'study',
                        count: widget.request_data.json()['study'],
                        barColor: charts.ColorUtil.fromDartColor(Colors.green),
                      ),
                      AppDownloads(
                        year: 'work',
                        count: widget.request_data.json()['work'],
                        barColor: charts.ColorUtil.fromDartColor(Colors.green),
                      ),
                    ];
                    setState(() {});
                  },
                  child: Container(
                    alignment: Alignment.center,
                    width: 100,
                    height: 100,
                    child: Text(
                      "Tap me",
                      textScaleFactor: 2,
                      style: TextStyle(color: Colors.blueGrey[600]),
                    ),
                  ),
                ),
              ),
            ),
            Expanded(
                child: Padding(
              padding: const EdgeInsets.all(30),
              child: MyBarChart(widget.data),
            )),
          ],
        ),
      ),
    );
  }
}
