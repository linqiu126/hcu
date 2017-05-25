<?php

use yii\helpers\Html;
use yii\widgets\DetailView;

/* @var $this yii\web\View */
/* @var $model app\models\Hcubfscstadatainfo */

$this->title = $model->StaType;
$this->params['breadcrumbs'][] = ['label' => 'Hcubfscstadatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcubfscstadatainfo-view">

    <h1><?= Html::encode($this->title) ?></h1>

    <p>
        <?= Html::a('Update', ['update', 'id' => $model->StaType], ['class' => 'btn btn-primary']) ?>
        <?= Html::a('Delete', ['delete', 'id' => $model->StaType], [
            'class' => 'btn btn-danger',
            'data' => [
                'confirm' => 'Are you sure you want to delete this item?',
                'method' => 'post',
            ],
        ]) ?>
    </p>

    <?= DetailView::widget([
        'model' => $model,
        'attributes' => [
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
        ],
    ]) ?>

</div>
