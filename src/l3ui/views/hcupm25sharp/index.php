<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\Hcupm25sharpSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcupm25sharps';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcupm25sharp-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcupm25sharp', ['create'], ['class' => 'btn btn-success']) ?>
    </p>

    <?= GridView::widget([
        'dataProvider' => $dataProvider,
        'filterModel' => $searchModel,
        'columns' => [
            ['class' => 'yii\grid\SerialColumn'],

            'sid',
            'pm25',

            ['class' => 'yii\grid\ActionColumn'],
        ],
    ]); ?>

</div>
