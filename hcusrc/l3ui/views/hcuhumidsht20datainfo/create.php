<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcuhumidsht20datainfo */

$this->title = 'Create Hcuhumidsht20datainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcuhumidsht20datainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuhumidsht20datainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
