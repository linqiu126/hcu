<?php

use yii\helpers\Html;

/* @var $this yii\web\View */
/* @var $model app\models\Hcutoxicgasmq135datainfo */

$this->title = 'Update Hcutoxicgasmq135datainfo: ' . ' ' . $model->sid;
$this->params['breadcrumbs'][] = ['label' => 'Hcutoxicgasmq135datainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = ['label' => $model->sid, 'url' => ['view', 'id' => $model->sid]];
$this->params['breadcrumbs'][] = 'Update';
?>
<div class="hcutoxicgasmq135datainfo-update">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
