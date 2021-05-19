import 'package:flutter/material.dart';

class Send_Activities_Page extends StatefulWidget {
  const Send_Activities_Page({Key key}) : super(key: key);

  @override
  _Send_Activities_PageState createState() => _Send_Activities_PageState();
}

class _Send_Activities_PageState extends State<Send_Activities_Page> {
  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Card(
          child: Container(
            color: Colors.blueGrey,
            height: 200,
            width: 200,
          ),
        )
      ],
    );
  }
}
