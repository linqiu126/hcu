<?php

use yii\helpers\Html;

/* @var $this yii\web\View */
/* @var $model app\models\Hcuhumidsht20datainfo */

$this->title = 'Update Hcuhumidsht20datainfo: ' . ' ' . $model->sid;
$this->params['breadcrumbs'][] = ['label' => 'Hcuhumidsht20datainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = ['label' => $model->sid, 'url' => ['view', 'id' => $model->sid]];
$this->params['breadcrumbs'][] = 'Update';
?>
<div class="hcuhumidsht20datainfo-update">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
