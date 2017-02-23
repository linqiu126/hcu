<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\HcubfscctrlcmdexgSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcubfscctrlcmdexgs';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcubfscctrlcmdexg-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcubfscctrlcmdexg', ['create'], ['class' => 'btn btn-success']) ?>
    </p>

    <?= GridView::widget([
        'dataProvider' => $dataProvider,
        'filterModel' => $searchModel,
        'columns' => [
            ['class' => 'yii\grid\SerialColumn'],

            'sid',
            'startexecuteflag',

            ['class' => 'yii\grid\ActionColumn'],
        ],
    ]); ?>

</div>
