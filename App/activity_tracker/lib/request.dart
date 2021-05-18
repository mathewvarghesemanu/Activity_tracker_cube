import 'dart:convert';
import 'package:http/http.dart' as http;
import 'dart:async';

class Album {
  final String exercise;
  final String read;
  final String study;
  final String work;

  Album({this.exercise, this.read, this.study, this.work});

  factory Album.fromJson(Map<String, dynamic> json) {
    return Album(
      exercise: json['exercise'],
      read: json['read'],
      study: json['study'],
      work: json['work'],
    );
  }
}

Future<Album> fetchAlbum() async {
  final response = await http.get(Uri.parse(
      'https://activitytracker.mak3r.space/get_activities/Mathew/aggregate_time'));

  if (response.statusCode == 200) {
    // If the server did return a 200 OK response,
    // then parse the JSON.
    return Album.fromJson(jsonDecode(response.body));
  } else {
    // If the server did not return a 200 OK response,
    // then throw an exception.
    throw Exception('Failed to load album');
  }
}
