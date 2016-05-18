<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcupm25sharpdatainfo */

$this->title = 'Create Hcupm25sharpdatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcupm25sharpdatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcupm25sharpdatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
