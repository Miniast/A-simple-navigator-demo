var pos;
var maxn = 61, maxm = 133;
var timer;


var nodeArray1 = ["请选择", "南门", "燕南园"];
var nodeArray2 = ["请选择", "教学楼", "教工食堂"];
var campusArray = ["请选择校区地图", "沙河校区", "西土城校区"];
var selectArray = [];
var methodsArray = ["请选择交通方式", "步行", "自行车"];
var priorityArray = ["请选择优先级", "距离优先", "时间优先"];
var optionX, optionY;
var dom = document.getElementById("showbox");
var myChart = echarts.init(dom);

var changefl = 0;
var model = 0, nodesNumber = 0, stage = 0;
var nodesArray = [];
var state = 0, pre;
var optionA, optionB, NowOption;
var NowNode = {
  nCampus: 0,
  nID: maxm,
  nName: "当前位置",
  x: 0.0,
  y: 0.0
}
var schoolbus = 0;

function Init() {
  for (var i = 0; i < campusArray.length; i++) {
    $("#campus").append("<option value='" + campusArray[i] + "'>" + campusArray[i] + "</option>");
  }
  schoolbus = 0;
  changefl = 0
  model = 0;
  $.ajax({
    url: '/api/getmap',
    method: 'get',
    success: function (res) {
      pos = res.nPoint;
      pos.push({
        nCampus: NowNode.nCampus,
        nID: maxm,
        nName: "当前位置",
        x: NowNode.x,
        y: NowNode.y
      })

      for (let i = 0; i < maxm; ++i)
        selectArray.push(pos[i].nName);

      $("#start").append("<option value=" + -1 + ">" + "请选择出发点" + "</option>");
      $("#target").append("<option value=" + -1 + ">" + "请选择目的地" + "</option>");

      for (let i = 0; i < maxn; i++) {
        $("#start").append("<option value=" + i + ">" + "沙河 — " + selectArray[i] + "</option>");
        $("#target").append("<option value=" + i + ">" + "沙河 — " + selectArray[i] + "</option>");
      }
      for (let i = maxn; i < maxm; i++) {
        $("#start").append("<option value=" + i + ">" + "西土城 — " + selectArray[i] + "</option>");
        $("#target").append("<option value=" + i + ">" + "西土城 — " + selectArray[i] + "</option>");
      }

      for (let i = 0; i < methodsArray.length; i++)
        $("#methods").append("<option value=" + i + ">" + methodsArray[i] + "</option>");
      for (let i = 0; i < priorityArray.length; i++)
        $("#priority").append("<option value=" + i + ">" + priorityArray[i] + "</option>");

      $.ajax({
        url: '/api/getedge',
        method: 'get',
        success: function (edgeres) {
          $.get('../svg/shahe.svg', function (svg) {
            echarts.registerMap('shahe', { svg: svg });
            optionX = {
              title: {
                text: '沙河校区校园导览系统',
                left: 'center',
                bottom: 10
              },
              tooltip: {
                triggerOn: "click",
                trigger: "item",
                confine: true,
                alwaysShowContent: false,
                hideDelay: 100
              },
              geo: {
                show: true,
                map: 'shahe',
                roam: true,
                emphasis: {
                  itemStyle: {
                    color: null
                  },
                  label: {
                    show: false
                  }
                }
              },
              series: [{
                name: '当前位置',
                type: 'scatter',
                coordinateSystem: 'geo',
                geoIndex: 0,
                symbol: '',
                symbolSize: 25,
                symbolRotate: 0,
                itemStyle: {
                  color: 'rgba(255, 0, 0, 1)',
                  opacity: 1
                },
                data: [],
                tooltip: {
                  formatter: function (params) {
                    var ret = params.data;
                    var json = {
                      "camp": ret.campusID,
                      "x": ret.value[0],
                      "y": ret.value[1]
                    }
                    var text = "";
                    text += '<div style="display:flex;flex-direction:row;">' +
                      '<div style="background-color:#03D16D;height:auto;width:10px;margin-right:5px;opacity:1"></div>' +
                      '<div style="display:flex;flex-direction:column;">';

                    text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "当前位置：" + '</li>' + "坐标：" + '(' + ret.value[0] + ',' + ret.value[1] + ')';
                    $.ajax({
                      type: 'POST',
                      data: json,
                      async: false,
                      dataType: 'json',
                      url: '/api/getnearby',
                      success: function (res) {
                        console.log(res);

                        text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "周边服务设施: " + '</li>';
                        for (let i = 0; i < res.cnt; ++i) {
                          text +=
                            '<span>' + res.nPoint[i].nName + '</span>' +
                            '<span style="margin-bottom:10px">' + "坐标：" + '(' + res.nPoint[i].x + ',' + res.nPoint[i].y + ')' + '</span>'
                        }
                        text += '</div>' + '</div>';
                      },
                      error: function (e) {
                        alert("tooltip fail");
                      }
                    });
                    return text;
                  }
                }
              },
              {
                name: '建筑 / 地标',
                type: 'scatter',
                coordinateSystem: 'geo',
                geoIndex: 0,
                symbol: 'image://http://127.0.0.1:8888/icon/structure.png',
                symbolSize: 15,
                cursor: 'pointer',
                itemStyle: {
                  color: 'rgba(255, 0, 0, 1)',
                  opacity: 1
                },
                data: [],
                tooltip: {
                  formatter: function (params) {
                    var text = "";
                    var ret = params.data;
                    text += '<div style="display:flex;flex-direction:row;">' +
                      '<div style="background-color:#ED1941;height:auto;width:10px;margin-right:5px;opacity:1"></div>' +
                      '<div style="display:flex;flex-direction:column;">';
                    text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "建筑 / 地标" + '</li>' +
                      '<span>' + "名称：" + ret.name + '</span>' +
                      '<span>' + "坐标：" + '(' + ret.value[0] + ',' + ret.value[1] + ')' + '</span>';
                    text += '</div>' + '</div>';
                    return text;
                  }
                }
              },
              {
                name: '目的地',
                type: 'scatter',
                coordinateSystem: 'geo',
                geoIndex: 0,
                symbol: 'image://http://127.0.0.1:8888/icon/flag.png',
                symbolSize: 25,
                symbolRotate: 0,
                itemStyle: {
                  color: 'rgba(255, 0, 0, 1)',
                  opacity: 1
                },
                data: [],
                tooltip: {
                  formatter: function (params) {
                    var text = "";
                    var ret = params.data;
                    text += '<div style="display:flex;flex-direction:row;">' +
                      '<div style="background-color:#ffc20e;height:auto;width:10px;margin-right:5px;opacity:1"></div>' +
                      '<div style="display:flex;flex-direction:column;">';
                    text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "目的地" + '</li>' +
                      '<span>' + "名称：" + ret.name + '</span>' +
                      '<span>' + "坐标：" + '(' + ret.value[0] + ',' + ret.value[1] + ')' + '</span>';
                    text += '</div>' + '</div>';
                    return text;
                  }
                }
              },
              {
                name: '步行路径',
                type: 'lines',
                coordinateSystem: 'geo',
                geoIndex: 0,
                symbol: 'emptyCircle',
                symbolSize: 5,
                itemStyle: {
                  color: '#ffc20e',
                  opacity: 1
                },
                lineStyle: {
                  color: '#ffc20e',
                  opacity: 0.9
                },
                data: [],
                tooltip: {
                  formatter: function (params) {
                    var text = "";
                    var ret = params.data;
                    text += '<div style="display:flex;flex-direction:row;">' +
                      '<div style="background-color:#8552a1;height:auto;width:10px;margin-right:5px;opacity:1"></div>' +
                      '<div style="display:flex;flex-direction:column;">';
                    text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "路径" + '</li>' +
                      '<span>' + "仅步行" + '</span>' +
                      '<span>' + "拥挤度：" + ret.crowd + '</span>';
                    text += '</div>' + '</div>';
                    return text;
                  }
                }
              },
              {
                name: '自行车路径',
                type: 'lines',
                coordinateSystem: 'geo',
                geoIndex: 0,
                symbol: 'emptyCircle',
                symbolSize: 5,
                itemStyle: {
                  color: 'rgba(190, 215, 66, 1)',
                  opacity: 1
                },
                lineStyle: {
                  color: 'rgba(190, 215, 66, 1)',
                  opacity: 0.9
                },
                data: [],
                tooltip: {
                  formatter: function (params) {
                    console.log(optionX.series[4].lineStyle.width);
                    console.log(params.data.crowd);
                    var text = "";
                    var ret = params.data;
                    text += '<div style="display:flex;flex-direction:row;">' +
                      '<div style="background-color:#9d9087;height:auto;width:10px;margin-right:5px;opacity:1"></div>' +
                      '<div style="display:flex;flex-direction:column;">';
                    text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "路径" + '</li>' +
                      '<span>' + "步行 / 自行车" + '</span>' +
                      '<span>' + "拥挤度：" + ret.crowd + '</span>';
                    text += '</div>' + '</div>';
                    return text;
                  }
                }
              }]
            };

            for (let i = 0; i < maxn; ++i) {
              optionX.series[1].data.push({
                value: [pos[i].x, pos[i].y],
                name: pos[i].nName,
                pId: i
              })
            }
            for (let i = 0; i < edgeres.eEdge.length; ++i) {
              if (edgeres.eEdge[i].eCampus == 0) {
                let k;
                if (edgeres.eEdge[i].eType == 0) k = 3;
                else k = 4;
                optionX.series[k].data.push({
                  coords: [
                    [pos[edgeres.eEdge[i].eStart].x, pos[edgeres.eEdge[i].eStart].y],
                    [pos[edgeres.eEdge[i].eEnd].x, pos[edgeres.eEdge[i].eEnd].y]
                  ],
                  crowd: edgeres.eEdge[i].eCrowd,
                  lineStyle: {
                    width: 10 - (edgeres.eEdge[i].eCrowd * 10)
                  }
                });
              }
            }
          });
          $.get('../svg/xitucheng.svg', function (svg) {
            echarts.registerMap('xitucheng', { svg: svg });
            optionY = {
              title: {
                text: '西土城校区校园导览系统',
                left: 'center',
                bottom: 10
              },
              tooltip: {
                triggerOn: "click",
                trigger: "item",
                confine: true,
                alwaysShowContent: false,
                hideDelay: 100
              },
              geo: {
                map: 'xitucheng',
                roam: true,
                emphasis: {
                  itemStyle: {
                    color: null
                  },
                  label: {
                    show: false
                  }
                }
              },
              series: [{
                name: '当前位置',
                type: 'scatter',
                coordinateSystem: 'geo',
                geoIndex: 0,
                symbol: '',
                symbolSize: 25,
                symbolRotate: 0,
                itemStyle: {
                  color: 'rgba(255, 0, 0, 1)',
                  opacity: 1
                },
                data: [],
                tooltip: {
                  formatter: function (params) {
                    var ret = params.data;
                    var json = {
                      "camp": ret.campusID,
                      "x": ret.value[0],
                      "y": ret.value[1]
                    }
                    var text = "";
                    text += '<div style="display:flex;flex-direction:row;">' +
                      '<div style="background-color:#03D16D;height:auto;width:10px;margin-right:5px;"></div>' +
                      '<div style="display:flex;flex-direction:column;">';

                    text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "当前位置：" + '</li>' + "坐标：" + '(' + ret.value[0] + ',' + ret.value[1] + ')';
                    $.ajax({
                      type: 'POST',
                      data: json,
                      dataType: 'json',
                      async: false,
                      url: '/api/getnearby',
                      success: function (res) {
                        console.log(res);

                        text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "周边服务设施: " + '</li>';
                        for (let i = 0; i < res.cnt; ++i) {
                          text +=
                            '<span>' + res.nPoint[i].nName + '</span>' +
                            '<span style="margin-bottom:10px">' + "坐标：" + '(' + res.nPoint[i].x + ',' + res.nPoint[i].y + ')' + '</span>'
                        }
                        text += '</div>' + '</div>';
                      },
                      error: function (e) {
                      }
                    });
                    return text;
                  }
                }
              },
              {
                name: '建筑 / 地标',
                type: 'scatter',
                coordinateSystem: 'geo',
                geoIndex: 0,
                symbol: 'image://http://127.0.0.1:8888/icon/structure.png',
                symbolSize: 15,
                cursor: 'pointer',
                itemStyle: {
                  color: 'rgba(255, 0, 0, 1)',
                  opacity: 1
                },
                data: [],
                tooltip: {
                  formatter: function (params) {
                    var text = "";
                    var ret = params.data;
                    text += '<div style="display:flex;flex-direction:row;">' +
                      '<div style="background-color:#ED1941;height:auto;width:10px;margin-right:5px;"></div>' +
                      '<div style="display:flex;flex-direction:column;">';
                    text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "建筑 / 地标" + '</li>' +
                      '<span>' + "名称：" + ret.name + '</span>' +
                      '<span>' + "坐标：" + '(' + ret.value[0] + ',' + ret.value[1] + ')' + '</span>';
                    text += '</div>' + '</div>';
                    return text;
                  }
                }
              },
              {
                name: '目的地',
                type: 'scatter',
                coordinateSystem: 'geo',
                geoIndex: 0,
                symbol: 'image://http://127.0.0.1:8888/icon/flag.png',
                symbolSize: 25,
                symbolRotate: 0,
                itemStyle: {
                  color: 'rgba(255, 0, 0, 1)',
                  opacity: 1
                },
                data: [],
                tooltip: {
                  formatter: function (params) {
                    var text = "";
                    var ret = params.data;
                    text += '<div style="display:flex;flex-direction:row;">' +
                      '<div style="background-color:#ffc20e;height:auto;width:10px;margin-right:5px;"></div>' +
                      '<div style="display:flex;flex-direction:column;">';
                    text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "目的地" + '</li>' +
                      '<span>' + "名称：" + ret.name + '</span>' +
                      '<span>' + "坐标：" + '(' + ret.value[0] + ',' + ret.value[1] + ')' + '</span>';
                    text += '</div>' + '</div>';
                    return text;
                  }
                }
              },
              {
                name: '步行路径',
                type: 'lines',
                coordinateSystem: 'geo',
                geoIndex: 0,
                symbol: 'emptyCircle',
                symbolSize: 5,
                itemStyle: {
                  color: '#ffc20e',
                  opacity: 1
                },
                lineStyle: {
                  color: '#ffc20e',
                  opacity: 0.9
                },
                data: [],
                tooltip: {
                  formatter: function (params) {
                    var text = "";
                    var ret = params.data;
                    text += '<div style="display:flex;flex-direction:row;">' +
                      '<div style="background-color:#8552a1;height:auto;width:10px;margin-right:5px;opacity:1"></div>' +
                      '<div style="display:flex;flex-direction:column;">';
                    text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "路径" + '</li>' +
                      '<span>' + "仅步行" + '</span>' +
                      '<span>' + "拥挤度：" + ret.crowd + '</span>';
                    text += '</div>' + '</div>';
                    return text;
                  }
                }
              },
              {
                name: '自行车路径',
                type: 'lines',
                coordinateSystem: 'geo',
                geoIndex: 0,
                symbol: 'emptyCircle',
                symbolSize: 5,
                itemStyle: {
                  color: 'rgba(190, 215, 66, 1)',
                  opacity: 1
                },
                lineStyle: {
                  color: 'rgba(190, 215, 66, 1)',
                  opacity: 0.9
                },
                data: [],
                tooltip: {
                  formatter: function (params) {
                    var text = "";
                    var ret = params.data;
                    text += '<div style="display:flex;flex-direction:row;">' +
                      '<div style="background-color:#9d9087;height:auto;width:10px;margin-right:5px;opacity:1"></div>' +
                      '<div style="display:flex;flex-direction:column;">';
                    text += '<li style="font-size:16px;margin:5px 0 5px 0;font-weight:550" >' + "路径" + '</li>' +
                      '<span>' + "步行 / 自行车" + '</span>' +
                      '<span>' + "拥挤度：" + ret.crowd + '</span>';
                    text += '</div>' + '</div>';
                    return text;
                  }
                }
              }]
            };

            for (let i = maxn; i < maxm; ++i) {
              optionY.series[1].data.push({
                value: [pos[i].x, pos[i].y],
                name: pos[i].nName,
                pId: i
              })
            }

            for (let i = 0; i < edgeres.eEdge.length; ++i) {
              if (edgeres.eEdge[i].eCampus == 1) {
                let k;
                if (edgeres.eEdge[i].eType == 0) k = 3;
                else k = 4;
                optionY.series[k].lineStyle.width = 10 - (edgeres.eEdge[i].eCrowd * 10);
                optionY.series[k].data.push({
                  coords: [
                    [pos[edgeres.eEdge[i].eStart].x, pos[edgeres.eEdge[i].eStart].y],
                    [pos[edgeres.eEdge[i].eEnd].x, pos[edgeres.eEdge[i].eEnd].y]
                  ],
                  crowd: edgeres.eEdge[i].eCrowd,
                  lineStyle: {
                    width: 10 - (edgeres.eEdge[i].eCrowd * 10)
                  }
                });
              }
            }
          });
        },
        error: function (e) {

        }
      });
    },
    error: function (err) {
    }
  });
}



function sendSubmit() {
  let st = $('#start option:selected').val(), en = $('#target option:selected').val();
  var json =
  {
    "pStrategy": $('#priority option:selected').val() - 1,
    "pType": $('#methods option:selected').val() - 1,
    "pName": "person",
    "pStCamp": 0,
    "pStId": st,
    "pEnCamp": 0,
    "pEnId": en,
    "pStartTime": 100,

    "isChange": 0,
    "pNowX": 0.0,
    "pNowY": 0.0,


    "isTravel": 0, // 0 原来的 1 新模式
    // 如果istravel为0，以下无意义
    // 如果istravel为1，"pStCamp", "pStId", "pEnCamp": 1, "pEnId"无意义
    "cnt": 0, // 总的点数，第一个表示起点，最后一个表示重点
    "pNode": []
  };
  if (st < maxn) json.pStCamp = 0;
  else json.pStCamp = 1;

  if (en < maxn) json.pEnCamp = 0;
  else json.pEnCamp = 1;

  if (model == 1) {
    json.isTravel = 1;
    json.pNowX = NowNode.x;
    json.pNowY = NowNode.y;
    json.cnt = nodesNumber;
    json.pNode = nodesArray;
  }
  if (changefl == 1) {
    json.isChange = 1;
    json.pNowX = NowNode.x;
    json.pNowY = NowNode.y;
  }

  $.ajax({
    type: 'POST',
    data: json,
    dataType: 'json',
    url: '/api/newperson',
    success: function (res) {
      console.log(res);
      layer.alert("寻路成功");
      var output = "";
      let prenode = maxm + 1;
      if (model == 1) {
        for (let i = 0; i < res.point.length - 1; ++i) {
          let now = pos[res.point[i].id];
          output += now.nName + ' -> ';
        }
        output += pos[res.point[res.point.length - 1].id].nName;
      }
      else {
        for (let i = 0; i < res.pLen - 1; ++i) {
          let now = pos[res.point[i].id];
          if (prenode == now) output += "乘坐校车 -> "
          else output += now.nName + ' -> ';
          prenode = now;
        }
        output += pos[res.point[res.pLen - 1].id].nName;
      }
      
      let hr = Math.floor(res.pTime / 3600), mi = Math.floor((res.pTime % 3600) / 60), se = res.pTime % 60;
      $("#road").text("推荐路径: " + output);
      $("#distance").text("路径长度: " + parseFloat(res.pLength.toFixed(2)) + ' m');
      $("#timecost").text("耗费时间: " + hr + '时' + mi + '分' + parseFloat(se.toFixed(2)) + '秒');
    },
    error: function (e) {
      layer.alert("寻路失败");
    }
  });

}

function sendStart() {
  var json = { "1": 1 };

  $.ajax({
    type: 'POST',
    data: json,
    dataType: 'json',
    url: '/api/start',
    success: function (data) {
      // alert("start success");
    },
    error: function (e) {
      // alert("start fail");
    }
  });
}

function sendPause() {
  var json = { "1": 1 };

  $.ajax({
    type: 'POST',
    data: json,
    dataType: 'json',
    url: '/api/pause',
    success: function (res) {
      // alert("sendPause success");
    },
    error: function (err) {
      // alert("sendPause fail");
    }
  });
}

function sendContinue() {
  var json = { "1": 1 };

  $.ajax({
    type: 'POST',
    data: json,
    dataType: 'json',
    url: '/api/continue',
    success: function (data) {
      // alert("sendContinue success");
    },
    error: function (e) {
      // alert("sendContinue fail");
    }
  });
}

function getNode() {
  timer = setInterval(function () {
    $.ajax({
      url: '/api/update',
      method: 'get',
      // async:false,
      success: function (res) {
        if (res.type == 3) {
          if (pre != 3) {//////坐校车
            schoolbus = 1;
            myChart.clear();
            $("#mapbox").append(("<div id = 'ball'> </div>"))
          }
        }
        else if (res.type == 0) {//////可结束
          state = 4;
          $("#Confirm").removeClass("btn-outline-warning");
          $("#Confirm").addClass("btn-outline-danger");
          $("#Confirm").text("结束");
          clearInterval(timer);
        }
        else {
          NowNode.x = res.point[0];
          NowNode.y = res.point[1];

          if (pre == 3) {//////结束坐校车
            schoolbus = 0;
            state = 2;
            $("#ball").remove();
            if (NowNode.nCampus == 0) {
              NowNode.nCampus = 1;
              optionY.series[0].data[0] = {
                campusID: NowNode.nCampus,
                value: res.point
              }
              optionY.series[0].symbol = optionX.series[0].symbol;
              myChart.setOption(optionY);
            }
            else {
              optionX.series[0].data[0] = {
                campusID: NowNode.nCampus,
                value: res.point
              }
              NowNode.nCampus = 1;
              optionX.series[0].symbol = optionY.series[0].symbol;
              myChart.setOption(optionX);
            }
          }
          else {///////正常运行
            NowOption = myChart.getOption();
            NowOption.series[0].data[0] = {
              campusID: NowNode.nCampus,
              value: res.point
            }
            myChart.setOption(NowOption);
          }
        }

        pre = res.type;

      },
      error: function (e) {
        clearInterval(timer);
        console.log('catch error on confirm button');
      }
    })
  }, 500);
}
function Refresh() {
  console.log(111);
  if (model == 1)
    $('#addednodes').text("已选取中间点数: 0");
  else {
    $('#addednodes').text("当前为普通模式");
  }
  optionX.series[0].data.length = 0;
  optionX.series[2].data.length = 0;
  optionY.series[0].data.length = 0;
  optionY.series[2].data.length = 0;

  $("#start").empty();
  $("#target").empty();
  $("#methods").empty();
  $("#priority").empty();

  $("#start").append("<option value=" + -1 + ">" + "请选择出发点" + "</option>");
  $("#target").append("<option value=" + -1 + ">" + "请选择目的地" + "</option>");

  for (let i = 0; i < maxn; i++) {
    $("#start").append("<option value=" + i + ">" + "沙河 — " + selectArray[i] + "</option>");
    $("#target").append("<option value=" + i + ">" + "沙河 — " + selectArray[i] + "</option>");
  }
  for (let i = maxn; i < maxm; i++) {
    $("#start").append("<option value=" + i + ">" + "西土城 — " + selectArray[i] + "</option>");
    $("#target").append("<option value=" + i + ">" + "西土城 — " + selectArray[i] + "</option>");
  }

  for (let i = 0; i < methodsArray.length; i++)
    $("#methods").append("<option value=" + i + ">" + methodsArray[i] + "</option>");
  for (let i = 0; i < priorityArray.length; i++)
    $("#priority").append("<option value=" + i + ">" + priorityArray[i] + "</option>");

  schoolbus = 0;
  nodesArray = [];
  nodesNumber = 0;
  state = 0;
  changefl = 0
  $()
  NowNode = {
    nCampus: 0,
    nID: maxm,
    nName: "当前位置",
    x: 0.0,
    y: 0.0
  }
}

function decrease() {
  var num = document.getElementById("getnumber");
  if (parseInt(num.value) <= 1)
    num.value = 1;
  else if (isNaN(num.value)) num.value = 1;
  else
    num.value = parseInt(num.value) - 1;

  var json = {
    "cnt": parseInt(num.value)
  }
  $.ajax({
    type: 'POST',
    data: json,
    dataType: 'json',
    url: '/api/multiset',
    success: function (res) {

    },
    error: function (e) {

    }
  });
}

function increase() {
  var num = document.getElementById("getnumber");
  if (isNaN(num.value)) num.value = 1;
  else
    num.value = parseInt(num.value) + 1;
  if (parseInt(num.value) > 500) num.value = 500;

  var json = {
    "cnt": parseInt(num.value)
  }
  $.ajax({
    type: 'POST',
    data: json,
    dataType: 'json',
    url: '/api/multiset',
    success: function (res) {

    },
    error: function (e) {

    }
  });
}

function getnumber() {
  var num = document.getElementById("getnumber");
  var value = num.value;

  if (value == "" || isNaN(value) || parseInt(value) <= 1)
    num.value = 1;
  if (parseInt(num.value) > 500) num.value = 500;

  var json = {
    "cnt": parseInt(num.value)
  }
  $.ajax({
    type: 'POST',
    data: json,
    dataType: 'json',
    url: '/api/multiset',
    success: function (res) {

    },
    error: function (e) {

    }
  });
}

function modelChange(ret) {
  if (ret.checked == true) {
    if (state != 0) {
      layer.alert("请在初始状态下进行模式切换");
      $("#modelsp").val([]);
      return;
    }
    model = 1; stage = 1;
    layer.alert("已进入选点寻路模式，请点击选取出发点");
    $("#addednodes").text("已选取中间点数: 0 ");
    $("#target").empty();
    $("#target").append("<option value=" + maxm + ">" + "请点击地图选取" + "</option>");
  }
  else {
    model = 0;
    layer.alert("已进入普通寻路模式");
    $("#addednodes").text("当前为普通寻路模式");
    Refresh();
  }
}


campus.addEventListener('change', function () {
  myChart = echarts.init(dom);

  if ($('#campus option:selected').val() == "请选择校区地图") myChart.clear();
  else {
    if ($('#campus option:selected').val() == "沙河校区") {
      myChart.setOption(optionX);
    }
    else if ($('#campus option:selected').val() == "西土城校区") {
      myChart.setOption(optionY);
    }
  }
});

Confirm.addEventListener('click', function () {
  switch (state) {//确认
    case 0: {
      let st = $('#start option:selected').val(), en = $('#target option:selected').val();

      if (st == -1) {
        layer.alert("未选择出发点");
        return;
      }
      if (en == -1) {
        if (model == 0) {
          layer.alert("未选择目的地");
          return;
        }
      }
      if ($('#methods option:selected').val() == 0) {
        layer.alert("未选择出行方式");
        return;
      }
      if ($('#priority option:selected').val() == 0) {
        layer.alert("未选择寻路优先级");
        return;
      }

      if (model == 1 && nodesNumber < 2) {
        layer.alert("请至少选择一个目的地！");
        return;
      }

      state = 1;
      $("#Confirm").removeClass("btn-outline-primary");
      $("#Confirm").addClass("btn-outline-success");
      $("#Confirm").text("开始");

      if (st != maxm) {
        if (st < maxn) NowNode.nCampus = 0;
        else NowNode.nCampus = 1;
      }


      if (st == maxm) {
        if (NowNode.nCampus == 0) {
          optionX.series[0].data.length = 0;
          optionX.series[0].data.push({
            campusID: NowNode.nCampus,
            value: [NowNode.x, NowNode.y]
          });
          if ($('#methods option:selected').val() == 1) {
            optionX.series[0].symbol = 'image://http://127.0.0.1:8888/icon/walk.png';
          }
          else {
            optionX.series[0].symbol = 'image://http://127.0.0.1:8888/icon/bike.png';
          }
          NowOption = optionX;
        }
        else {
          optionY.series[0].data.length = 0;
          optionY.series[0].data.push({
            campusID: NowNode.nCampus,
            value: [NowNode.x, NowNode.y]
          });
          if ($('#methods option:selected').val() == 1) {
            optionY.series[0].symbol = 'image://http://127.0.0.1:8888/icon/walk.png';
          }
          else {
            optionY.series[0].symbol = 'image://http://127.0.0.1:8888/icon/bike.png';
          }
          NowOption = optionY;
        }
      }
      else if (st < maxn) {
        optionX.series[0].data.length = 0;
        optionX.series[0].data.push({
          campusID: NowNode.nCampus,
          value: [pos[st].x, pos[st].y]
        });
        if ($('#methods option:selected').val() == 1) {
          optionX.series[0].symbol = 'image://http://127.0.0.1:8888/icon/walk.png';
        }
        else {
          optionX.series[0].symbol = 'image://http://127.0.0.1:8888/icon/bike.png';
        }
        NowOption = optionX;
      }
      else {
        optionY.series[0].data.length = 0;
        optionY.series[0].data.push({
          campusID: NowNode.nCampus,
          value: [pos[st].x, pos[st].y]
        });
        if ($('#methods option:selected').val() == 1) {
          optionY.series[0].symbol = 'image://http://127.0.0.1:8888/icon/walk.png';
        }
        else {
          optionY.series[0].symbol = 'image://http://127.0.0.1:8888/icon/bike.png';
        }
        NowOption = optionY;
      }


      if (en < maxn) {
        optionX.series[2].data.length = 0;
        optionX.series[2].data.push({
          campusID: 0,
          name: pos[en].nName,
          value: [pos[en].x, pos[en].y]
        });
      }
      else {
        optionY.series[2].data.length = 0;
        optionY.series[2].data.push({
          campusID: 1,
          name: pos[en].nName,
          value: [pos[en].x, pos[en].y]
        });
      }


      myChart.setOption(NowOption);

      sendSubmit();
      break;
    }
    case 1: {//开始运行
      state = 2;
      $("#Confirm").removeClass("btn-outline-success");
      $("#Confirm").addClass("btn-outline-warning");
      $("#Confirm").text("暂停");
      sendStart();
      $("#start").empty();
      $("#start").append("<option value=" + maxm + ">" + "当前位置" + "</option>");
      getNode();
      break;
    }
    case 2: {//暂停
      if (schoolbus == 1) {
        layer.alert("乘坐校车中，不可暂停");
        return;
      }
      state = 3;
      $("#Confirm").removeClass("btn-outline-warning");
      $("#Confirm").addClass("btn-outline-info");
      $("#Confirm").text("继续")
      sendPause();
      clearInterval(timer);
      break;
    }
    case 3: {//继续运行
      state = 2;
      $("#Confirm").removeClass("btn-outline-info");
      $("#Confirm").addClass("btn-outline-warning");
      $("#Confirm").text("暂停");
      sendContinue();
      getNode();
      break;
    }
    case 4: {//结束
      state = 0;
      $("#Confirm").removeClass("btn-outline-danger");
      $("#Confirm").addClass("btn-outline-primary");
      $("#Confirm").text("确认");
      Refresh();
      break;
    }
  }
});

Rechange.addEventListener('click', function () {
  if (state != 3) {
    layer.alert("请在暂停时设置新的目的地、交通方式和优先级");
  }
  else {
    if (model == 1) {
      layer.alert("选点（多目标）模式不可更改目的地");
      return;
    }
    var json = {
      "content": "1"
    }
    $.ajax({
      type: 'POST',
      data: json,
      dataType: 'json',
      url: '/api/change',
      success: function (res) {
        state = 0;
        changefl = 1;
        layer.alert("更改请求成功，请在更改后点击确认")
        $("#Confirm").removeClass("btn-outline-info");
        $("#Confirm").addClass("btn-outline-primary");
        $("#Confirm").text("确认");
        clearInterval(timer);
      },
      error: function (e) {
        layer.alert("更改失败");
      }
    });
  }
});

myChart.off('click').on('click', function (param) {
  if (param.seriesName === "建筑 / 地标" && model == 1) {
    if (stage == 1) {
      layer.confirm('选择' + param.data.name + '作为出发点？', { icon: 3, title: '提示' }, function (index) {
        $('#start').val(param.data.pId);
        nodesNumber++;
        let k;
        if (param.data.pId < maxn) k = 0;
        else k = 1;
        nodesArray.push({
          pCamp: k,
          pId: param.data.pId
        });
        stage = 2;
        layer.alert("请点击选取中间点/目的地")
        layer.close(index);
      });
    }
    else if (stage == 2) {
      layer.confirm('选择' + param.data.name + '作为中间点/目的地？', { icon: 3, title: '提示' }, function (index) {
        nodesNumber++;
        $('#addednodes').text("已选取中间点数: " + (nodesNumber - 1));
        let k;
        if (param.data.pId < maxn) k = 0;
        else k = 1;
        nodesArray.push({
          pCamp: k,
          pId: param.data.pId
        });
        layer.close(index);
      });
    }
  }
});

Init();