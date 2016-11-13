<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcutoxicgasmq135datainfo */

$this->title = 'Create Hcutoxicgasmq135datainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcutoxicgasmq135datainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcutoxicgasmq135datainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
