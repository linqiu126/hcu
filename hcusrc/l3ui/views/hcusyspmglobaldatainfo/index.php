<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\HcusyspmglobaldatainfoSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcusyspmglobaldatainfos';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcusyspmglobaldatainfo-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcusyspmglobaldatainfo', ['create'], ['class' => 'btn btn-success']) ?>
    </p>

    <?= GridView::widget([
        'dataProvider' => $dataProvider,
        'filterModel' => $searchModel,
        'columns' => [
            ['class' => 'yii\grid\SerialColumn'],

            'sid',
            'taskhcuvmec',
            'taskhwinvec',
            'tasksyspmec',
            'taskmodbusec',
            'taskcloudvelaec',
            'taskavorionec',
            'taskspsvirgoec',
            'taskhsmmpec',
            'taskemcec',
            'taskpm25ec',
            'taskwinddirec',
            'taskwindspdec',
            'tasktempec',
            'taskhumidec',
            'tasknoiseec',
            'taskairprsec',
            'taskco1ec',
            'tasklightstrec',
            'taskalcoholec',
            'taskhchoec',
            'tasktoxicgasec',
            'restartcnt',
            'cloudvelaconncnt',
            'cloudveladisccnt',
            'clouddatatimeoutcnt:datetime',
            'timestamp:datetime',

            ['class' => 'yii\grid\ActionColumn'],
        ],
    ]); ?>

</div>
