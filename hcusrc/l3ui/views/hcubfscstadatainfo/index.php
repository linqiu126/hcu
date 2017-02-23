<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\HcubfscstadatainfoSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcubfscstadatainfos';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcubfscstadatainfo-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcubfscstadatainfo', ['create'], ['class' => 'btn btn-success']) ?>
    </p>

    <?= GridView::widget([
        'dataProvider' => $dataProvider,
        'filterModel' => $searchModel,
        'columns' => [
            ['class' => 'yii\grid\SerialColumn'],

            'StaType',
            'timestamp:datetime',
            'wsIncMatCnt',
            'wsIncMatWgt',
            'wsCombTimes:datetime',
            'wsTttTimes:datetime',
            'wsTgvTimes:datetime',
            'wsTttMatCnt',
            'wsTgvMatCnt',
            'wsTttMatWgt',
            'wsTgvMatWgt',
            'wsAvgTttTimes:datetime',
            'wsAvgTttMatCnt',
            'wsAvgTttMatWgt',

            ['class' => 'yii\grid\ActionColumn'],
        ],
    ]); ?>

</div>