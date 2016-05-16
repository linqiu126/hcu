<?php

use yii\helpers\Html;

/* @var $this yii\web\View */
/* @var $model app\models\Hcupm25sharp */

$this->title = 'Update Hcupm25sharp: ' . ' ' . $model->sid;
$this->params['breadcrumbs'][] = ['label' => 'Hcupm25sharps', 'url' => ['index']];
$this->params['breadcrumbs'][] = ['label' => $model->sid, 'url' => ['view', 'id' => $model->sid]];
$this->params['breadcrumbs'][] = 'Update';
?>
<div class="hcupm25sharp-update">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
