import 'package:charts_flutter/flutter.dart' as charts;
import 'package:flutter/material.dart';
import 'dart:math';

class AppDownloads {
  final String year;

  final int count;

  final charts.Color barColor;

  AppDownloads({
    @required this.year,
    @required this.count,
    @required this.barColor,
  });
}

class MyBarChart extends StatefulWidget {
  final List<AppDownloads> data;

  MyBarChart(this.data);

  @override
  _MyBarChartState createState() => _MyBarChartState();
}

class _MyBarChartState extends State<MyBarChart> {
  @override
  Widget build(BuildContext context) {
    List<charts.Series<AppDownloads, String>> series = [
      charts.Series(
          id: 'AppDownloads',
          data: widget.data,
          domainFn: (AppDownloads downloads, _) => downloads.year,
          measureFn: (AppDownloads downloads, _) => downloads.count,
          colorFn: (AppDownloads downloads, _) => downloads.barColor)
    ];

    return charts.BarChart(
      series,
      animate: true,
      barGroupingType: charts.BarGroupingType.groupedStacked,
    );
  }
}
