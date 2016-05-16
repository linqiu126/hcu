<?php

use yii\helpers\Html;
use yii\widgets\DetailView;

/* @var $this yii\web\View */
/* @var $model app\models\Hcupm25sharp */

$this->title = $model->sid;
$this->params['breadcrumbs'][] = ['label' => 'Hcupm25sharps', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcupm25sharp-view">

    <h1><?= Html::encode($this->title) ?></h1>

    <p>
        <?= Html::a('Update', ['update', 'id' => $model->sid], ['class' => 'btn btn-primary']) ?>
        <?= Html::a('Delete', ['delete', 'id' => $model->sid], [
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
            'sid',
            'pm25',
        ],
    ]) ?>

</div>
