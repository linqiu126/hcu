<?php

/* @var $this yii\web\View */

$this->title = '风速传感器图表展示';

/*//sample #1
use miloschuman\highcharts\Highcharts;
use yii\web\JsExpression;

echo Highcharts::widget([
    'scripts' => [
        'modules/exporting',
        'themes/grid-light',
    ],
    'options' => [
        'title' => [
            'text' => 'Combination chart',
        ],
        'xAxis' => [
            'categories' => ['Apples', 'Oranges', 'Pears', 'Bananas', 'Plums'],
        ],
        'labels' => [
            'items' => [
                [
                    'html' => 'Total fruit consumption',
                    'style' => [
                        'left' => '50px',
                        'top' => '18px',
                        'color' => new JsExpression('(Highcharts.theme && Highcharts.theme.textColor) || "black"'),
                    ],
                ],
            ],
        ],
        'series' => [
            [
                'type' => 'column',
                'name' => 'Jane',
                'data' => [3, 2, 1, 3, 4],
            ],
            [
                'type' => 'column',
                'name' => 'John',
                'data' => [2, 3, 5, 7, 6],
            ],
            [
                'type' => 'column',
                'name' => 'Joe',
                'data' => [4, 3, 3, 9, 0],
            ],
            [
                'type' => 'spline',
                'name' => 'Average',
                'data' => [3, 2.67, 3, 6.33, 3.33],
                'marker' => [
                    'lineWidth' => 2,
                    'lineColor' => new JsExpression('Highcharts.getOptions().colors[3]'),
                    'fillColor' => 'white',
                ],
            ],
            [
                'type' => 'pie',
                'name' => 'Total consumption',
                'data' => [
                    [
                        'name' => 'Jane',
                        'y' => 13,
                        'color' => new JsExpression('Highcharts.getOptions().colors[0]'), // Jane's color
                    ],
                    [
                        'name' => 'John',
                        'y' => 23,
                        'color' => new JsExpression('Highcharts.getOptions().colors[1]'), // John's color
                    ],
                    [
                        'name' => 'Joe',
                        'y' => 19,
                        'color' => new JsExpression('Highcharts.getOptions().colors[2]'), // Joe's color
                    ],
                ],
                'center' => [100, 80],
                'size' => 100,
                'showInLegend' => false,
                'dataLabels' => [
                    'enabled' => false,
                ],
            ],
        ],
    ]
]);*/

/*
//sample #2
use miloschuman\highcharts\Highcharts;
 
echo Highcharts::widget([
   'options' => [
      'title' => ['text' => 'Fruit Consumption'],
      'xAxis' => [
         'categories' => ['Apples', 'Bananas', 'Oranges']
      ],
      'yAxis' => [
         'title' => ['text' => 'Fruit eaten']
      ],
      'series' => [
         ['name' => 'Jane', 'data' => [1, 0, 4]],
         ['name' => 'John', 'data' => [5, 7, 3]]
      ]
   ]
]);*/

/*//sample #3
use miloschuman\highcharts\Highmaps;
use yii\web\JsExpression;


 // To use Highcharts Map Collection, we must register those files separately.
 // The 'depends' option ensures that the main Highmaps script gets loaded first.
$this->registerJsFile('http://code.highcharts.com/mapdata/countries/de/de-all.js', [
    'depends' => 'miloschuman\highcharts\HighchartsAsset'
]);

echo Highmaps::widget([
    'options' => [
        'title' => [
            'text' => 'Highmaps basic demo',
        ],
        'mapNavigation' => [
            'enabled' => true,
            'buttonOptions' => [
                'verticalAlign' => 'bottom',
            ]
        ],
        'colorAxis' => [
            'min' => 0,
        ],
        'series' => [
            [
                'data' => [
                    ['hc-key' => 'de-ni', 'value' => 0],
                    ['hc-key' => 'de-hb', 'value' => 1],
                    ['hc-key' => 'de-sh', 'value' => 2],
                    ['hc-key' => 'de-be', 'value' => 3],
                    ['hc-key' => 'de-mv', 'value' => 4],
                    ['hc-key' => 'de-hh', 'value' => 5],
                    ['hc-key' => 'de-rp', 'value' => 6],
                    ['hc-key' => 'de-sl', 'value' => 7],
                    ['hc-key' => 'de-by', 'value' => 8],
                    ['hc-key' => 'de-th', 'value' => 9],
                    ['hc-key' => 'de-st', 'value' => 10],
                    ['hc-key' => 'de-sn', 'value' => 11],
                    ['hc-key' => 'de-br', 'value' => 12],
                    ['hc-key' => 'de-nw', 'value' => 13],
                    ['hc-key' => 'de-bw', 'value' => 14],
                    ['hc-key' => 'de-he', 'value' => 15],
                ],
                'mapData' => new JsExpression('Highcharts.maps["countries/de/de-all"]'),
                'joinBy' => 'hc-key',
                'name' => 'Random data',
                'states' => [
                    'hover' => [
                        'color' => '#BADA55',
                    ]
                ],
                'dataLabels' => [
                    'enabled' => true,
                    'format' => '{point.name}',
                ]
            ]
        ]
    ]
]);*/

//sample #4
use miloschuman\highcharts\Highstock;
use yii\web\JsExpression;

//$this->registerJs('$.getJSON("//www.highcharts.com/samples/data/jsonp.php?filename=aapl-c.json&callback=?", myCallbackFunction);');
$this->registerJs('$.getJSON( "/l3ui/web/index.php?r=hcuwindspddatainfo/getallhcuwindspddatainfo&callback=?" , myCallbackFunction);');

echo Highstock::widget([
    // The highcharts initialization statement will be wrapped in a function
    // named 'mycallbackFunction' with one parameter: data.
    'callback' => 'myCallbackFunction',
    'options' => [
        'rangeSelector' => [
            'inputEnabled' => new JsExpression('$("#container").width() > 480'),
            'selected' => 1
        ],
        'title' => [
            'text' => '风速传感器图表展示'
        ],
        'series' => [
            [
                'name' => '风速传感器数值',
                'data' => new JsExpression('data'), // Here we use the callback parameter, data
                'type' => 'areaspline',
                'threshold' => null,
                'tooltip' => [
                    'valueDecimals' => 2
                ],
                'fillColor' => [
                    'linearGradient' => [
                        'x1' => 0,
                        'y1' => 0,
                        'x2' => 0,
                        'y2' => 1
                    ],
                    'stops' => [
                        [0, new JsExpression('Highcharts.getOptions().colors[0]')],
                        [1, new JsExpression('Highcharts.Color(Highcharts.getOptions().colors[0]).setOpacity(0).get("rgba")')]
                    ]
                ]
            ]
        ]
    ]
]);

?>