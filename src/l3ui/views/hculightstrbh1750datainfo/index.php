<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\Hculightstrbh1750datainfoSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hculightstrbh1750datainfos';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hculightstrbh1750datainfo-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hculightstrbh1750datainfo', ['create'], ['class' => 'btn btn-success']) ?>
    </p>

    <?= GridView::widget([
        'dataProvider' => $dataProvider,
        'filterModel' => $searchModel,
        'columns' => [
            ['class' => 'yii\grid\SerialColumn'],

            'sid',
            'deviceid',
            'timestamp:datetime',
            'dataformat',
            'lightstrvalue',

            ['class' => 'yii\grid\ActionColumn'],
        ],
    ]); ?>

</div>
