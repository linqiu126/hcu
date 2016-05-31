<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\Hcutoxicgasmq135datainfoSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcutoxicgasmq135datainfos';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcutoxicgasmq135datainfo-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcutoxicgasmq135datainfo', ['create'], ['class' => 'btn btn-success']) ?>
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
            'toxicgasvalue',

            ['class' => 'yii\grid\ActionColumn'],
        ],
    ]); ?>

</div>
